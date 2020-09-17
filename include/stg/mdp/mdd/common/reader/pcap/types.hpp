#pragma once

#include <cstddef>

#include <net/ethernet.h>
#include <netinet/ip.h>
#include <netinet/tcp.h>
#include <netinet/udp.h>

namespace stg::mdp::mdd::common::reader::pcap
{

struct package_header
{
    const struct ip* ip_hdr;
    const struct udphdr* udp_hdr;
    const struct ether_header* eth_hdr;
};

struct udp_addr
{
    char src_ip[INET_ADDRSTRLEN];
    char dst_ip[INET_ADDRSTRLEN];
};

struct udp_port
{
    std::size_t src_port;
    std::size_t dst_port;
};

struct udp_header
{
    struct udp_addr addr;
    struct udp_port port;
};

}