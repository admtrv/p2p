/*
 *  arp.cpp
 */

#include "arp.h"

void arp_header::to_buf(arp_header& arp, unsigned char* buffer)
{
    // Hardware Type (2 bytes)
    uint16_t hw_type_network = htons(ARP_HW_TYPE_ETHERNET);
    memcpy(buffer, &hw_type_network, sizeof(hw_type_network));

    // Protocol Type (2 bytes)
    uint16_t proto_type_network = htons(ARP_PROTOCOL_ADDR_TYPE_IPV4);
    memcpy(buffer + 2, &proto_type_network, sizeof(proto_type_network));

    // Hardware Address Length (1 byte)
    buffer[4] = ARP_HW_ADDR_LEN;

    // Protocol Address Length (1 byte)
    buffer[5] = ARP_PROTOCOL_ADDR_LEN;

    // Opcode (2 bytes)
    uint16_t opcode_network = htons(arp.operation);
    memcpy(buffer + 6, &opcode_network, sizeof(opcode_network));

    // Sender Hardware Address (6 bytes)
    memcpy(buffer + 8, arp.src_hw_addr, ARP_HW_ADDR_LEN);

    // Sender Protocol Address (4 bytes)
    uint32_t src_proto_addr_network = arp.src_protocol_addr;
    memcpy(buffer + 14, &src_proto_addr_network, sizeof(src_proto_addr_network));

    // Target Hardware Address (6 bytes)
    memcpy(buffer + 18, arp.tgt_hw_addr, ARP_HW_ADDR_LEN);

    // Target Protocol Address (4 bytes)
    uint32_t tgt_proto_addr_network = arp.tgt_protocol_addr;
    memcpy(buffer + 24, &tgt_proto_addr_network, sizeof(tgt_proto_addr_network));
}

void arp_header::from_buf(arp_header& arp, unsigned char* buffer)
{
    // Hardware Type (2 bytes)
    arp.hw_addr_type = ntohs(*(uint16_t*)(buffer));

    // Protocol Type (2 bytes)
    arp.protocol_addr_type = ntohs(*(uint16_t*)(buffer + 2));

    // Hardware Address Length (1 byte)
    arp.hw_addr_len = buffer[4];

    // Protocol Address Length (1 byte)
    arp.protocol_addr_len = buffer[5];

    // Opcode (2 bytes)
    arp.operation = ntohs(*(uint16_t*)(buffer + 6));

    // Sender Hardware Address (6 bytes)
    memcpy(arp.src_hw_addr, buffer + 8, ARP_HW_ADDR_LEN);

    // Sender Protocol Address (4 bytes)
    arp.src_protocol_addr = ntohl(*(uint32_t*)(buffer + 14));

    // Target Hardware Address (6 bytes)
    memcpy(arp.tgt_hw_addr, buffer + 18, ARP_HW_ADDR_LEN);

    // Target Protocol Address (4 bytes)
    arp.tgt_protocol_addr = ntohl(*(uint32_t*)(buffer + 24));
}
