#include "ip.h"

void to_buf(ip_protocol& ip, unsigned char* buffer) {
    buffer[0] = ip.ip_header.version_ihl;

    buffer[1] = ip.ip_header.type_of_service;

    buffer[2] = (ip.ip_header.total_length >> 8) & 0xFF;
    buffer[3] = ip.ip_header.total_length & 0xFF;

    buffer[4] = (ip.ip_header.identification >> 8) & 0xFF;
    buffer[5] = ip.ip_header.identification & 0xFF;

    buffer[6] = (ip.ip_header.flags_offset >> 8) & 0xFF;
    buffer[7] = ip.ip_header.flags_offset & 0xFF;

    buffer[8] = ip.ip_header.time_to_live;

    buffer[9] = ip.ip_header.protocol;

    buffer[10] = (ip.ip_header.header_checksum >> 8) & 0xFF;
    buffer[11] = ip.ip_header.header_checksum & 0xFF;

    memcpy(buffer + 12, &ip.ip_header.source_ip, sizeof(ip.ip_header.source_ip));
    memcpy(buffer + 16, &ip.ip_header.dest_ip, sizeof(ip.ip_header.dest_ip));

    memcpy(buffer + sizeof(ip_header), ip.payload, ip.ip_header.total_length - sizeof(ip_header));
}

void from_buf(ip_protocol& ip, unsigned char* buffer) {
    ip.ip_header.version_ihl = buffer[0];
    ip.ip_header.type_of_service = buffer[1];
    ip.ip_header.total_length = (buffer[2] << 8) | buffer[3];
    ip.ip_header.identification = (buffer[4] << 8) | buffer[5];
    ip.ip_header.flags_offset = (buffer[6] << 8) | buffer[7];
    ip.ip_header.time_to_live = buffer[8];
    ip.ip_header.protocol = buffer[9];
    ip.ip_header.header_checksum = (buffer[10] << 8) | buffer[11];
    memcpy(&ip.ip_header.source_ip, buffer + 12, sizeof(ip.ip_header.source_ip));
    memcpy(&ip.ip_header.dest_ip, buffer + 16, sizeof(ip.ip_header.dest_ip));

    ip.payload = buffer + sizeof(ip_header);
}
