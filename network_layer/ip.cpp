/*
 *  ip.cpp
 */

#include "ip.h"

void ip_protocol::to_buf(ip_protocol& ip, unsigned char* buffer) {
    buffer[0] = ip.ip_hdr.version_ihl;

    buffer[1] = ip.ip_hdr.type_of_service;

    buffer[2] = (ip.ip_hdr.total_length >> 8) & 0xFF;
    buffer[3] = ip.ip_hdr.total_length & 0xFF;

    buffer[4] = (ip.ip_hdr.identification >> 8) & 0xFF;
    buffer[5] = ip.ip_hdr.identification & 0xFF;

    buffer[6] = (ip.ip_hdr.flags_offset >> 8) & 0xFF;
    buffer[7] = ip.ip_hdr.flags_offset & 0xFF;

    buffer[8] = ip.ip_hdr.time_to_live;

    buffer[9] = ip.ip_hdr.protocol;

    buffer[10] = (ip.ip_hdr.header_checksum >> 8) & 0xFF;
    buffer[11] = ip.ip_hdr.header_checksum & 0xFF;

    memcpy(buffer + 12, &ip.ip_hdr.source_ip, sizeof(ip.ip_hdr.source_ip));
    memcpy(buffer + 16, &ip.ip_hdr.dest_ip, sizeof(ip.ip_hdr.dest_ip));

    memcpy(buffer + sizeof(ip_hdr), ip.payload, ip.ip_hdr.total_length - sizeof(ip_hdr));
}

void ip_protocol::from_buf(ip_protocol& ip, unsigned char* buffer) {
    ip.ip_hdr.version_ihl = buffer[0];
    ip.ip_hdr.type_of_service = buffer[1];
    ip.ip_hdr.total_length = (buffer[2] << 8) | buffer[3];
    ip.ip_hdr.identification = (buffer[4] << 8) | buffer[5];
    ip.ip_hdr.flags_offset = (buffer[6] << 8) | buffer[7];
    ip.ip_hdr.time_to_live = buffer[8];
    ip.ip_hdr.protocol = buffer[9];
    ip.ip_hdr.header_checksum = (buffer[10] << 8) | buffer[11];
    memcpy(&ip.ip_hdr.source_ip, buffer + 12, sizeof(ip.ip_hdr.source_ip));
    memcpy(&ip.ip_hdr.dest_ip, buffer + 16, sizeof(ip.ip_hdr.dest_ip));

    ip.payload = buffer + sizeof(ip_hdr);
}

bool ip_protocol::get_ip_addr(int sock, const char* interface, uint32_t* ip_addr)
{
    struct ifreq ifr;
    strncpy(ifr.ifr_name, interface, IFNAMSIZ - 1);

    if (ioctl(sock, SIOCGIFADDR, &ifr) == 0)
    {
        struct sockaddr_in* ipaddr = (struct sockaddr_in*)&ifr.ifr_addr;
        *ip_addr = ipaddr->sin_addr.s_addr;
        return true;
    }
    else
    {
        perror("ioctl");
        *ip_addr = 0;
        return false;
    }
}

