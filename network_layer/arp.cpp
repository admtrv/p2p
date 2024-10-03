#include "arp.h"

void to_buf(arp_header& arp, unsigned char* buffer) {
    buffer[0] = arp.hw_addr_type >> 8;
    buffer[1] = arp.hw_addr_type & 0xFF;

    buffer[2] = arp.protocol_addr_type >> 8;
    buffer[3] = arp.protocol_addr_type & 0xFF;

    buffer[4] = arp.hw_addr_len;

    buffer[5] = arp.protocol_addr_len;

    buffer[6] = arp.operation >> 8;
    buffer[7] = arp.operation & 0xFF;

    memcpy(buffer + 8, arp.src_hw_addr, ETH_ALEN);

    buffer[14] = (arp.src_protocol_addr >> 24) & 0xFF;
    buffer[15] = (arp.src_protocol_addr >> 16) & 0xFF;
    buffer[16] = (arp.src_protocol_addr >> 8) & 0xFF;
    buffer[17] = arp.src_protocol_addr & 0xFF;

    memcpy(buffer + 18, arp.tgt_hw_addr, ETH_ALEN);

    buffer[24] = (arp.tgt_protocol_addr >> 24) & 0xFF;
    buffer[25] = (arp.tgt_protocol_addr >> 16) & 0xFF;
    buffer[26] = (arp.tgt_protocol_addr >> 8) & 0xFF;
    buffer[27] = arp.tgt_protocol_addr & 0xFF;
}

void from_buf(arp_header& arp, unsigned char* buffer) {
    arp.hw_addr_type = (buffer[0] << 8) | buffer[1];
    arp.protocol_addr_type = (buffer[2] << 8) | buffer[3];
    arp.hw_addr_len = buffer[4];
    arp.protocol_addr_len = buffer[5];
    arp.operation = (buffer[6] << 8) | buffer[7];
    memcpy(arp.src_hw_addr, buffer + 8, ETH_ALEN);
    arp.src_protocol_addr = (buffer[14] << 24) | (buffer[15] << 16) | (buffer[16] << 8) | buffer[17];
    memcpy(arp.tgt_hw_addr, buffer + 18, ETH_ALEN);
    arp.tgt_protocol_addr = (buffer[24] << 24) | (buffer[25] << 16) | (buffer[26] << 8) | buffer[27];
}
