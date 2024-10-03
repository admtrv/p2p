#include "node.h"

Node::Node(const char* interface) : intf(interface) {
    // Создание raw сокета
    sock_desc = socket(AF_PACKET, SOCK_RAW, htons(ETH_P_ALL));
    if (sock_desc < 0) {
        perror("socket");
        exit(EXIT_FAILURE);
    }

    // Настройка структуры сокета
    memset(&sock_addr, 0, sizeof(sock_addr));
    sock_addr.sll_family = AF_PACKET;
    sock_addr.sll_protocol = htons(ETH_P_ALL);
    sock_addr.sll_ifindex = if_nametoindex(interface);

    if (bind(sock_desc, (struct sockaddr*)&sock_addr, sizeof(sock_addr)) < 0) {
        perror("bind");
        close(sock_desc);
        exit(EXIT_FAILURE);
    }

    // Получаем MAC-адрес источника
    get_mac_address(interface);
}

// Метод для получения MAC-адреса
void Node::get_mac_address(const char* interface) {
    int sock = socket(AF_INET, SOCK_DGRAM, 0);
    struct ifreq ifr;

    strncpy(ifr.ifr_name, interface, IFNAMSIZ - 1);
    if (ioctl(sock, SIOCGIFHWADDR, &ifr) == 0) {
        memcpy(src_mac, ifr.ifr_hwaddr.sa_data, ETH_ALEN);
    } else {
        perror("ioctl");
    }
    close(sock);
}

void Node::send_data(const char* payload) {
    // Подготовка отправляемого пакета
    unsigned char send_buffer[ETH_FRAME_LEN];
    memset(send_buffer, 0, sizeof(send_buffer));

    // Установка Ethernet заголовка
    struct ethhdr* eth_header = (struct ethhdr*)send_buffer;
    memcpy(eth_header->h_source, src_mac, ETH_ALEN);
    memcpy(eth_header->h_dest, dest_mac, ETH_ALEN);
    eth_header->h_proto = htons(ETH_P_IP);

    // Установка полезной нагрузки
    memcpy(send_buffer + sizeof(struct ethhdr), payload, strlen(payload));

    // Отправка пакета
    ssize_t num_bytes_sent = sendto(sock_desc, send_buffer, sizeof(struct ethhdr) + strlen(payload), 0,
                                    (struct sockaddr*)&sock_addr, sizeof(sock_addr));
    if (num_bytes_sent < 0) {
        perror("sendto");
        close(sock_desc);
        return;
    }

    std::cout << "sent " << num_bytes_sent << " bytes" << std::endl;
}

void Node::receive_data() {
    // Получение данных
    ssize_t num_bytes_received = recvfrom(sock_desc, buff, sizeof(buff), 0, nullptr, nullptr);
    if (num_bytes_received < 0) {
        perror("recvfrom");
        close(sock_desc);
        return;
    }

    // Указатель на Ethernet заголовок
    struct ethhdr* eth_header = (struct ethhdr*)buff;

    std::cout << "received " << num_bytes_received << " bytes from "
              << std::hex << (int)eth_header->h_source[0] << ":"
              << (int)eth_header->h_source[1] << ":"
              << (int)eth_header->h_source[2] << ":"
              << (int)eth_header->h_source[3] << ":"
              << (int)eth_header->h_source[4] << ":"
              << (int)eth_header->h_source[5] << std::dec
              << " with payload " << (buff + sizeof(struct ethhdr)) << std::endl;

    // Обновление MAC-адреса назначения
    memcpy(dest_mac, eth_header->h_source, ETH_ALEN);
}

