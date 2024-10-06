/*
 *  node.cpp
 */

#include "node.h"

Node::Node() : src_mac{0}, dest_mac{0}, running(false)
{
    unsigned char null_eth_addr[ETH_ALEN] = NULL_ETH_ADDR;
    unsigned char broadcast_eth_addr[ETH_ALEN] = BROADCAST_ETH_ADDR;

    std::copy(null_eth_addr, null_eth_addr + ETH_ALEN, src_mac);
    std::copy(broadcast_eth_addr, broadcast_eth_addr + ETH_ALEN, dest_mac);
}

Node::~Node()
{
    stop_receiving();
    close(sock_desc);
}

void Node::init_inf(const char* interface)
{
    intf = interface;

    sock_desc = socket(AF_PACKET, SOCK_RAW, htons(ETH_P_ALL));
    if (sock_desc < 0)
    {
        perror("socket");
        exit(EXIT_FAILURE);
    }

    int flags = fcntl(sock_desc, F_GETFL, 0);
    fcntl(sock_desc, F_SETFL, flags | O_NONBLOCK);

    memset(&sock_addr, 0, sizeof(sock_addr));
    sock_addr.sll_family = AF_PACKET;
    sock_addr.sll_protocol = htons(ETH_P_ALL);
    sock_addr.sll_ifindex = if_nametoindex(interface);

    if (bind(sock_desc, (struct sockaddr*)&sock_addr, sizeof(sock_addr)) < 0)
    {
        perror("bind");
        close(sock_desc);
        exit(EXIT_FAILURE);
    }

    unsigned char eth_addr[ETH_ALEN];
    bool success_eth = ethernet_protocol::get_eth_addr(sock_desc, interface, eth_addr);
    if (!success_eth)
    {
        std::cerr << "fail to get eth addr from inf " << interface << std::endl;
        close(sock_desc);
        exit(EXIT_FAILURE);
    }
    memcpy(src_mac, eth_addr, ETH_ALEN);

    uint32_t ip_addr;
    bool success_ip = ip_protocol::get_ip_addr(sock_desc, interface, &ip_addr);
    if (!success_ip)
    {
        std::cerr << "fail to get ip addr from inf " << interface << std::endl;
        close(sock_desc);
        exit(EXIT_FAILURE);
    }
    src_ip = inet_ntoa(*(struct in_addr*)&ip_addr);

    start_receiving();
}

void Node::start_receiving()
{
    running = true;
    receive_thread = std::thread(&Node::receive_loop, this);
}

void Node::stop_receiving()
{
    running = false;

    if (sock_desc != -1)
    {
        close(sock_desc);
        sock_desc = -1;
    }

    if (receive_thread.joinable())
    {
        receive_thread.join();
    }
}

void Node::receive_loop()
{
    while (running)
    {
        if (sock_desc == -1)
        {
            break;
        }

        receive_packet();
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
}

void Node::receive_packet()
{
    fd_set read_fds;
    FD_ZERO(&read_fds);
    FD_SET(sock_desc, &read_fds);

    struct timeval timeout;
    timeout.tv_sec = 0;
    timeout.tv_usec = 100000;

    int result = select(sock_desc + 1, &read_fds, nullptr, nullptr, &timeout);

    if (result > 0 && FD_ISSET(sock_desc, &read_fds))
    {
        ssize_t num_bytes_received = recvfrom(sock_desc, buff, sizeof(buff), 0, nullptr, nullptr);

        if (num_bytes_received < 0)
        {
            perror("recvfrom");
        }
        else
        {
            // std::cout << "received " << static_cast<size_t>(num_bytes_received) << " bytes" << std::endl;
            fsm_process(buff, static_cast<size_t>(num_bytes_received));
        }
    }
    else if (result < 0)
    {
        perror("select");
    }
}

void Node::fsm_process(unsigned char* packet, size_t length)
{
    ethernet_protocol ethernet;
    ethernet_protocol::from_buf(ethernet, packet);

    if (ethernet.eth_hdr.ether_type == ETHERNET_TYPE_ARP)
    {
        arp_header arp_hdr;
        arp_header::from_buf(arp_hdr, packet + sizeof(ethernet_header));

        if (arp_hdr.operation == ARP_OPERATION_REQUEST)
        {
            process_arp_request(arp_hdr);
        }
        else if (arp_hdr.operation == ARP_OPERATION_REPLY)
        {
            process_arp_reply(arp_hdr);
        }
    }
}


void Node::send_packet(unsigned char* buf, size_t length)
{
    ssize_t num_bytes_sent = sendto(sock_desc, buf, length, 0, (struct sockaddr*)&sock_addr, sizeof(sock_addr));
    if (num_bytes_sent < 0)
    {
        perror("sendto");
    }
    else
    {
        // std::cout << "sent " << num_bytes_sent << " bytes" << std::endl;
    }
}

void Node::send_arp_request()
{
    if (dest_ip.empty() || dest_port == 0 || src_port == 0)
    {
        std::cerr << "src port/dest ip/port not set" << std::endl;
        return;
    }

    unsigned char arp_request[ETH_FRAME_LEN];
    memset(arp_request, 0, sizeof(arp_request));

    // Заполнение Ethernet заголовка
    ethernet_header eth_hdr;
    memcpy(eth_hdr.src_mac, src_mac, ETH_ALEN);
    unsigned char broadcast_eth_addr[ETH_ALEN] = BROADCAST_ETH_ADDR;
    memcpy(eth_hdr.dest_mac, broadcast_eth_addr, ETH_ALEN);
    eth_hdr.ether_type = htons(ETHERNET_TYPE_ARP);

    memcpy(arp_request, &eth_hdr, sizeof(ethernet_header));

    // Заполнение ARP-запрос
    arp_header arp_hdr;
    arp_hdr.hw_addr_type = ARP_HW_TYPE_ETHERNET;
    arp_hdr.protocol_addr_type = ARP_PROTOCOL_ADDR_TYPE_IPV4;
    arp_hdr.hw_addr_len = ETH_ALEN;
    arp_hdr.protocol_addr_len = ARP_PROTOCOL_ADDR_LEN;
    arp_hdr.operation = ARP_OPERATION_REQUEST;

    memcpy(arp_hdr.src_hw_addr, src_mac, ETH_ALEN);
    arp_hdr.src_protocol_addr = inet_addr(src_ip.c_str());

    memcpy(arp_hdr.tgt_hw_addr, broadcast_eth_addr, ETH_ALEN);
    arp_hdr.tgt_protocol_addr = inet_addr(dest_ip.c_str());

    // Копируем ARP-заголовок в буфер после Ethernet-заголовка
    arp_header::to_buf(arp_hdr, arp_request + sizeof(ethernet_header));

    // Отправка пакета
    send_packet(arp_request, sizeof(ethernet_header) + sizeof(arp_header));
}

void Node::send_arp_reply()
{
    if (dest_ip.empty() || dest_port == 0 || src_port == 0)
    {
        std::cerr << "src port/dest ip/port not set" << std::endl;
        return;
    }

    unsigned char arp_request[ETH_FRAME_LEN];
    memset(arp_request, 0, sizeof(arp_request));

    // Заполнение Ethernet заголовка
    ethernet_header eth_hdr;
    memcpy(eth_hdr.src_mac, src_mac, ETH_ALEN);
    memcpy(eth_hdr.dest_mac, dest_mac, ETH_ALEN);
    eth_hdr.ether_type = htons(ETHERNET_TYPE_ARP);

    memcpy(arp_request, &eth_hdr, sizeof(ethernet_header));

    // Заполнение ARP-ответ
    arp_header arp_hdr;
    arp_hdr.hw_addr_type = ARP_HW_TYPE_ETHERNET;
    arp_hdr.protocol_addr_type = ARP_PROTOCOL_ADDR_TYPE_IPV4;
    arp_hdr.hw_addr_len = ETH_ALEN;
    arp_hdr.protocol_addr_len = ARP_PROTOCOL_ADDR_LEN;
    arp_hdr.operation = ARP_OPERATION_REPLY;

    memcpy(arp_hdr.src_hw_addr, src_mac, ETH_ALEN);
    arp_hdr.src_protocol_addr = inet_addr(src_ip.c_str());

    memcpy(arp_hdr.tgt_hw_addr, dest_mac, ETH_ALEN);
    arp_hdr.tgt_protocol_addr = inet_addr(dest_ip.c_str());

    // Копируем ARP-заголовок в буфер после Ethernet-заголовка
    arp_header::to_buf(arp_hdr, arp_request + sizeof(ethernet_header));

    // Отправка пакета
    send_packet(arp_request, sizeof(ethernet_header) + sizeof(arp_header));
}

void Node::process_arp_request(arp_header& request)
{
    //std::cout << "received arp request from mac " << request.src_hw_addr << std::endl;
    memcpy(dest_mac, request.src_hw_addr, ETH_ALEN);
    send_arp_reply();

    struct in_addr ip_struct;
    ip_struct.s_addr = ntohl(request.src_protocol_addr);
    std::cout << "ip-" << inet_ntoa(ip_struct) << " equals mac-" << ethernet_protocol::eth_to_str(request.src_hw_addr) << std::endl;
}

void Node::process_arp_reply(arp_header& reply)
{
    //std::cout << "received arp reply from mac " << reply.src_hw_addr << std::endl;
    memcpy(dest_mac, reply.src_hw_addr, ETH_ALEN);

    struct in_addr ip_struct;
    ip_struct.s_addr = ntohl(reply.src_protocol_addr);
    std::cout << "ip-" << inet_ntoa(ip_struct) << " equals mac-" << ethernet_protocol::eth_to_str(reply.src_hw_addr) << std::endl;
}