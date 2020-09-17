#pragma once

#include <iostream>
#include <string>
#include <fstream>
#include <exception>

#include <stg/ai/ai.hpp>
#include <stg/mdp/mdd/conf/mcast/types.hpp>

namespace stg::mdp::mdd::mcast::conf{

class mcast_conf
{
public:
    mcast_conf(const json& j):
               mcast_sender( j["multicast"]["server"] ),
               //asm_recv( j["multicast"]["client"]["ASM"] ),
               //ssm_recv( j["multicast"]["client"]["SSM"] ),
               //mcast_receiver( j["multicast"]["client"] ),
               json_addresses( j["address"] ),
               delay_ms( j["delay_ms"] ),
               delay_us( j["delay_us"] ),
               package( j["package"] ),
               dump_file( j["dump_file"] ),
               cloop( j["loop"] ),
               port( j["port"] ),
               cores( j["cores"] ),
               threads( j["threads"] ),
               ip_addr_start( j["ip_addr_start"] )
    {}

    const struct multicast_conf::server mcast_sender;
    //const struct multicast_conf::client::ASM asm_recv;
    //const struct multicast_conf::client::SSM ssm_recv;
    //const struct multicast_conf::client mcast_receiver;

    //! Test address count
    const std::size_t json_addresses;

    //! Delay in millisec
    const std::size_t delay_ms;

    //! Delay in microsec
    const std::size_t delay_us;

    //! Number of package send by each channel per microsec
    const std::size_t package;

    //! Dump filename
    const std::string dump_file;

    const std::size_t cloop;

    //! Dissemination port
    const std::size_t port;

    //! Dissemination core count
    const std::size_t cores;

    //! Dissemination thread count in each core
    const std::size_t threads;

    const std::string ip_addr_start;
};


} // namespace stg::mdp::mdd::mcast::conf
