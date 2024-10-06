/*
 *  node.h
 */

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
#include <vector>
#include <tuple>
#include <thread>
#include <atomic>
#include <arpa/inet.h>
#include <fcntl.h>

#include "../link_layer/ethernet.h"
#include "../network_layer/arp.h"
#include "../network_layer/ip.h"

class Node {
public:
    Node();
    ~Node();

    void init_inf(const char* interface);

    void fsm_process(unsigned char* packet, size_t length);

    void send_message(const char* payload);
    void send_file(const char* file_name);

    void send_packet(unsigned char* buf, size_t length); // Abstract function to send packet
    void receive_packet();  // Abstract function to receive packet

    void start_receiving();
    void stop_receiving();

    void send_arp_request();
    void send_arp_reply();

    void process_arp_request(arp_header& request);
    void process_arp_reply(arp_header& reply);

    inline void set_port(uint16_t port)
    {
        src_port = port;
    }

    inline void set_dest(std::string& ip, uint16_t port)
    {
        dest_ip = ip;
        dest_port = port;
    }

private:
    /* Socket params */
    int sock_desc;
    struct sockaddr_ll sock_addr;

    /* Interface params */
    const char* intf;

    /* Source node params */
    uint16_t src_port;
    unsigned char src_mac[ETH_ALEN];
    std::string src_ip;

    /* Destination node params */
    uint16_t dest_port;
    unsigned char dest_mac[ETH_ALEN];
    std::string dest_ip;

    /* Buffer params*/
    unsigned char buff[2048];

    void receive_loop();
    std::atomic<bool> running;
    std::thread receive_thread;
};

