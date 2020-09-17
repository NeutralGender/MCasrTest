#pragma once

#include <iostream>
#include <string>
#include <exception>
#include <map>
#include <tuple>

#include <arpa/inet.h>
#include <sys/socket.h>
#include <net/ethernet.h>
#include <netinet/ip.h>
#include <netinet/tcp.h>

#include <pcap.h>

#include <stg/mdp/mdd/common/reader/interface.hpp>
#include <stg/mdp/mdd/common/reader/pcap/types.hpp>

namespace stg::mdp::mdd::common::reader::pcap
{

using stg::mdp::mdd::common::reader::read_api;

/**
 * @brief Low-level module for pcap files reading
 * 
 */
class pcap_reader: 
   public read_api
{
public:
    pcap_reader() {}

    virtual ~pcap_reader() {}

    /**
     * @brief Open file
     * 
     * @param open_name file path
     * @return true if file was opened
     * @return false if cannot open file
     */
    virtual bool open( const std::string& open_name ) override;

    /**
     * @brief Reading pcap file
     * 
     * @param data container for <address:data> from file
     */
    virtual std::tuple<u_int32_t, u_int8_t*> read() override;

    /**
     * @brief Get the data object
     */
    virtual addr_data& get_data() override
    { return (filtered_pac); }

protected:

private:
    /**
     * @brief 
     * 
     * @param header captured package header
     * @param package full captured package
     * @return std::tuple<u_int32_t, u_char*>
     */
    std::tuple<u_int32_t, u_char*> packet_handler( const struct pcap_pkthdr* header, const u_char* package );

    /**
     * @brief Compiling string into filter program
     */
    void compile_filter();

    /**
     * @brief Set the filter object
     */
    void set_filter();

    /**
     * @brief Convert string to u_int32
     * 
     * @param str_ip 
     * @return u_int32_t 
     */
    u_int32_t str_to_uint32(const std::string& str_ip)
    { 
        u_int32_t uint_ip;
        inet_pton(AF_INET, str_ip.c_str(), &uint_ip );

        return (uint_ip);
    }

    /**
     * @brief Pcap handler
     */
    pcap_t* m_pcapfd;

    /**
     * @brief Struct contains headers
     */
    struct pcap_pkthdr* header;

    /**
     * @brief Package contents
     */
    const u_char* package_data;

    /**
     * @brief Multicast filter condition
     */
    static constexpr char filter_expr[] = "ip[16] >= 224";

    //! Compiled program
    struct bpf_program filter;

    //! Container for <address:data> from file
    addr_data filtered_pac;

    //! Buffer for errrs strings
    char err_buf[PCAP_ERRBUF_SIZE];
};

std::tuple<u_int32_t, u_int8_t*> pcap_reader::packet_handler( const struct pcap_pkthdr* header, const u_char* package )
{
    struct package_header p_hdr;
    struct udp_header udp_hdr;
    u_char* data;
    std::string data_str;
    u_int32_t data_len;
    
    p_hdr.eth_hdr = (struct ether_header*)(package);
    if( ntohs(p_hdr.eth_hdr->ether_type) == ETHERTYPE_IP )
    {
        p_hdr.ip_hdr = (struct ip*)(package + sizeof(ether_header));
        
        inet_ntop(AF_INET, &(p_hdr.ip_hdr->ip_src), udp_hdr.addr.src_ip, INET_ADDRSTRLEN);
        inet_ntop(AF_INET, &(p_hdr.ip_hdr->ip_dst), udp_hdr.addr.dst_ip, INET_ADDRSTRLEN);

        if( p_hdr.ip_hdr->ip_p == IPPROTO_UDP )
        {
            p_hdr.udp_hdr = (struct udphdr*)(package + sizeof(ether_header) + sizeof(ip));
            
            udp_hdr.port.src_port = ntohs(p_hdr.udp_hdr->uh_sport);
            udp_hdr.port.dst_port = ntohs(p_hdr.udp_hdr->uh_dport);

            data = (u_char*)(package + sizeof(ether_header) + sizeof(ip) + sizeof(udphdr));
            data_len = header->len - ( (sizeof(ether_header)) + (sizeof(ip)) + (sizeof(udphdr)) );

            return  std::make_tuple(data_len, data);
        }
    }

    return std::make_tuple(NULL,nullptr);
}

void pcap_reader::compile_filter()
{
    try
    {
        if( pcap_compile( m_pcapfd, &filter, (char*)filter_expr, 0, 0 ) < 0 )
            throw std::runtime_error( pcap_geterr(m_pcapfd) );
    }
    catch(const std::runtime_error& r)
    {
        std::cerr << r.what() << '\n';
    }
}

void pcap_reader::set_filter()
{
    try
    {
        if( pcap_setfilter( m_pcapfd, &filter ) < 0 )
            throw std::runtime_error( pcap_geterr(m_pcapfd) );
    }
    catch(const std::runtime_error& r)
    {
        std::cerr << r.what() << '\n';
    }
    
}

[[nodiscard]]
bool pcap_reader::open( const std::string& open_name )
{
    m_pcapfd = pcap_open_offline(open_name.c_str(), err_buf);
    std::cout << err_buf << std::endl;
    return ( m_pcapfd = pcap_open_offline(open_name.c_str(), err_buf) ) == nullptr ? false : true;
}

std::tuple<u_int32_t, u_int8_t*> pcap_reader::read()
{
    return ( pcap_next_ex( m_pcapfd, &header, &package_data ) == -2 ) ?
                std::make_pair(-2, nullptr) : packet_handler(header, package_data);

}


}