#include "ethernet.h"

void to_buf(ethernet_protocol& ethernet, unsigned char* buffer, size_t payload_length) {
    memcpy(buffer, ethernet.etehernet_header.dest_mac, ETH_ALEN);
    memcpy(buffer + ETH_ALEN, ethernet.etehernet_header.src_mac, ETH_ALEN);

    buffer[ETH_ALEN * 2] = (ethernet.etehernet_header.ether_type >> 8) & 0xFF;
    buffer[ETH_ALEN * 2 + 1] = ethernet.etehernet_header.ether_type & 0xFF;

    memcpy(buffer + ETHERNET_HEADER_LEN, ethernet.payload, payload_length);
}


void from_buf(ethernet_protocol& ethernet, unsigned char* buffer) {
    memcpy(ethernet.etehernet_header.dest_mac, buffer, ETH_ALEN);
    memcpy(ethernet.etehernet_header.src_mac, buffer + ETH_ALEN, ETH_ALEN);

    ethernet.etehernet_header.ether_type = (buffer[ETH_ALEN * 2] << 8) | buffer[ETH_ALEN * 2 + 1];

    ethernet.payload = buffer + ETHERNET_HEADER_LEN;
}

