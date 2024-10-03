#pragma once

#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/ioctl.h>
#include <netpacket/packet.h>
#include <unistd.h>
#include <cstring>
#include <linux/if_ether.h>
#include <net/if.h>
#include <iostream>
#include <string>

class Node {
public:
    Node(const char* interface);

    void send_data(const char* payload);
    void receive_data();

private:
    void get_mac_address(const char* interface);

    /* Socket params */
    int sock_desc;
    struct sockaddr_ll sock_addr;

    /* Interface params */
    const char* intf;

    /* Source node params */
    uint16_t src_port;
    unsigned char src_mac[ETH_ALEN]{0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

    /* Destination node params */
    uint16_t dest_port;
    unsigned char dest_mac[ETH_ALEN]{0xff, 0xff, 0xff, 0xff, 0xff, 0xff};

    /* Buffer params*/
    unsigned char buff[2048];
};
