/*
 * RFC:  791
 *
 *                          INTERNET PROTOCOL
 *                        DARPA INTERNET PROGRAM
 *                        PROTOCOL SPECIFICATION
 *
 *                            September 1981
 *
 *                             prepared for:
 *              Defense Advanced Research Projects Agency
 *               Information Processing Techniques Office
 *                        1400 Wilson Boulevard
 *                      Arlington, Virginia  22209
 *
 *                                  by:
 *                    Information Sciences Institute
 *                  University of Southern California
 *                          4676 Admiralty Way
 *                  Marina del Rey, California  90291
 *
 * September 1981
 *                                                      Internet Protocol
 * PREFACE:
 * This document specifies the DoD Standard Internet Protocol.  This
 * document is based on six earlier editions of the ARPA Internet Protocol
 * Specification, and the present text draws heavily from them.  There have
 * been many contributors to this work both in terms of concepts and in
 * terms of text.  This edition revises aspects of addressing, error
 * handling, option codes, and the security, precedence, compartments, and
 * handling restriction features of the internet protocol.
 *
 * Reference: https://datatracker.ietf.org/doc/html/rfc791
 */

#pragma once

#include <cstdint>
#include <cstring>
#include <iostream>
#include <linux/if_ether.h>
#include <net/if.h>
#include <sys/ioctl.h>
#include <netinet/in.h>

#define IP_HEAD_IPV4 0x4
#define IP_HEAD_INTERNET_HEAD_LEN 0x5
#define IP_HEAD_FLAG_D 0x4000
#define IP_HEAD_FLAG_M 0x2000
#define IP_HEAD_PROTOCOL_UDP 17

struct ip_header {
    uint8_t  version_ihl;
    uint8_t  type_of_service;
    uint16_t total_length;
    uint16_t identification;
    uint16_t flags_offset;
    uint8_t  time_to_live;
    uint8_t  protocol;
    uint16_t header_checksum;
    uint32_t source_ip;
    uint32_t dest_ip;
};

struct ip_protocol {
    ip_header ip_hdr;
    unsigned char* payload;

    void to_buf(ip_protocol& ip, unsigned char* buffer);
    void from_buf(ip_protocol& ip, unsigned char* buffer);

    uint16_t calculate_checksum(ip_protocol& ip);
    bool check_checksum(ip_protocol& ip);

    static bool get_ip_addr(int sock, const char* interface, uint32_t* ip_addr);
};
