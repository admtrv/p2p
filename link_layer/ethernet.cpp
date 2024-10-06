/*
 *  ethernet.cpp
 */

#include <iomanip>
#include "ethernet.h"

void ethernet_protocol::to_buf(ethernet_protocol& ethernet, unsigned char* buffer, size_t payload_length)
{
    memcpy(buffer, ethernet.eth_hdr.dest_mac, ETH_ALEN);
    memcpy(buffer + ETH_ALEN, ethernet.eth_hdr.src_mac, ETH_ALEN);

    buffer[ETH_ALEN * 2] = (ethernet.eth_hdr.ether_type >> 8) & 0xFF;
    buffer[ETH_ALEN * 2 + 1] = ethernet.eth_hdr.ether_type & 0xFF;

    memcpy(buffer + ETHERNET_HEADER_LEN, ethernet.payload, payload_length);
}

void ethernet_protocol::from_buf(ethernet_protocol& ethernet, unsigned char* buffer)
{
    memcpy(ethernet.eth_hdr.dest_mac, buffer, ETH_ALEN);
    memcpy(ethernet.eth_hdr.src_mac, buffer + ETH_ALEN, ETH_ALEN);

    ethernet.eth_hdr.ether_type = (buffer[ETH_ALEN * 2] << 8) | buffer[ETH_ALEN * 2 + 1];

    ethernet.payload = buffer + ETHERNET_HEADER_LEN;
}

bool ethernet_protocol::get_eth_addr(int sock, const char* interface, unsigned char* eth_addr)
{
    struct ifreq ifr;
    strncpy(ifr.ifr_name, interface, IFNAMSIZ - 1);

    if (ioctl(sock, SIOCGIFHWADDR, &ifr) == 0)
    {
        std::memcpy(eth_addr, ifr.ifr_hwaddr.sa_data, ETH_ALEN);
        return true; // Успех
    }
    else
    {
        perror("ioctl");
        unsigned char null_addr[ETH_ALEN] = NULL_ETH_ADDR;
        std::memcpy(eth_addr, null_addr, ETH_ALEN);
        return false; // Ошибка
    }
}

std::string ethernet_protocol::eth_to_str(const unsigned char* eth_addr)
{
    std::ostringstream oss;
    oss << std::hex << std::setfill('0');

    for (int i = 0; i < ETH_ALEN; ++i)
    {
        oss << std::setw(2) << static_cast<int>(eth_addr[i]);
        if (i != ETH_ALEN - 1)
        {
            oss << ":";
        }
    }

    return oss.str();
}

