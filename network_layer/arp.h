/*
 * Internet Research Task Force (IRTF)                          RJ Atkinson
 * Request for Comments: 6747                                    Consultant
 * Category: Experimental                                         SN Bhatti
 * ISSN: 2070-1721                                            U. St Andrews
 *                                                            November 2012
 *
 *                  Address Resolution Protocol (ARP)
 *    for the Identifier-Locator Network Protocol for IPv4 (ILNPv4)
 *
 *  Abstract:
 *  This document defines an Address Resolution Protocol (ARP) extension
 *  to support the Identifier-Locator Network Protocol for IPv4 (ILNPv4).
 *  ILNP is an experimental, evolutionary enhancement to IP.  This
 *  document is a product of the IRTF Routing Research Group.
 *
 *  Status of This Memo:
 *  This document is not an Internet Standards Track specification; it is
 *  published for examination, experimental implementation, and
 *  evaluation.
 *
 *  This document defines an Experimental Protocol for the Internet
 *  community.  This document is a product of the Internet Research Task
 *  Force (IRTF).  The IRTF publishes the results of Internet-related
 *  research and development activities.  These results might not be
 *  suitable for deployment.  This RFC represents the individual
 *  opinion(s) of one or more members of the Routing Research Group of
 *  the Internet Research Task Force (IRTF).  Documents approved for
 *  publication by the IRSG are not a candidate for any level of Internet
 *  Standard; see Section 2 of RFC 5741.
 *
 *
 * Reference: https://datatracker.ietf.org/doc/html/rfc6747
 */

#pragma once

#include <cstdint>
#include <cstring>
#include <iostream>
#include <arpa/inet.h>
#include <linux/if_ether.h>

#define ARP_HW_TYPE_ETHERNET 1
#define ARP_PROTOCOL_ADDR_TYPE_IPV4 0x0800
#define ARP_PROTOCOL_ADDR_LEN 4
#define ARP_OPERATION_REQUEST 1
#define ARP_OPERATION_REPLY 2

struct arp_header {
    uint16_t hw_addr_type;
    uint16_t protocol_addr_type;
    uint8_t hw_addr_len;
    uint8_t protocol_addr_len;
    uint16_t operation;

    unsigned char src_hw_addr[ETH_ALEN];
    uint32_t src_protocol_addr;
    unsigned char tgt_hw_addr[ETH_ALEN];
    uint32_t tgt_protocol_addr;
};

void to_buf(arp_header& arp, unsigned char* buffer);
void from_buf(arp_header& arp, unsigned char* buffer);
