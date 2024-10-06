/*
 * Network Working Group                                       Mike O'Dell
 * Internet Draft                                               Jed Kaplan
 * Expiration Date: November 1999				  UUNET Technologies, Inc.
 *
 *                              							 Ted Schroeder
 *							                       Alteon WebSystems, Inc.
 *
 *							                                    P.J. Singh
 *							                          Packet Engines, Inc.
 *
 *							                                  Jennifer Hsu
 *							                        Juniper Networks, Inc.
 *
 *	          Extended Ethernet Frame Size Support
 *
 *  Status of this Memo:
 *	This document is an Internet-Draft and is in full conformance with
 *	all provisions of Section 10 of RFC2026.
 *
 *	Internet-Drafts are working documents of the Internet Engineering
 *	Task Force (IETF), its areas, and its working groups. Note that
 *	other groups may also distribute working documents as Internet-
 *	Drafts.
 *
 *	Internet-Drafts are draft documents valid for a maximum of six months
 *	and may be updated, replaced, or obsoleted by other documents at any
 *	time. It is inappropriate to use Internet-Drafts as reference
 *	material or to cite them other than as "work in progress."
 *
 *  Reference: https://www.ietf.org/proceedings/46/I-D/draft-kaplan-isis-ext-eth-00.txt
 */

#pragma once

#include <cstdint>
#include <cstring>
#include <iostream>
#include <linux/if_ether.h>
#include <net/if.h>
#include <tuple>
#include <sys/ioctl.h>

#define ETHERNET_TYPE_IP 0x0800
#define ETHERNET_TYPE_ARP 0x0806
#define ETHERNET_HEADER_LEN 14

#define NULL_ETH_ADDR {0x00, 0x00, 0x00, 0x00, 0x00, 0x00}
#define BROADCAST_ETH_ADDR {0xff, 0xff, 0xff, 0xff, 0xff, 0xff}

// Ethernet header structure
struct ethernet_header {
    unsigned char dest_mac[ETH_ALEN];
    unsigned char src_mac[ETH_ALEN];
    uint16_t ether_type;
};

struct ethernet_protocol {
    ethernet_header eth_hdr;
    unsigned char* payload;

    void to_buf(ethernet_protocol& ethernet, unsigned char* buffer, size_t payload_length);
    static void from_buf(ethernet_protocol& ethernet, unsigned char* buffer);
    static bool get_eth_addr(int sock, const char* interface, unsigned char* eth_addr);
    static std::string eth_to_str(const unsigned char* eth_addr);
};

