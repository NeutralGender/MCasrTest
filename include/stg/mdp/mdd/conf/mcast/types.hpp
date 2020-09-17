#pragma once

#include <stg/ai/ai.hpp>

#include <cstddef>
#include <string>

namespace stg::mdp::mdd::mcast::conf
{


struct multicast_conf
{
    struct server
    {
        std::string local;
    };
};


void from_json( const json& j, multicast_conf::server& srv )
{
    j.at("local").get_to(srv.local);
}

} // namespace stg::mdp::mdd::conf

