#pragma once

#include <cstddef>
#include <string>

#include <stg/ai/ai.hpp>

namespace stg::mdp::mdd::conf::auth_conf
{

struct auth
{
    std::string local;
    std::string server_pub_key;
    std::string client_login;
    std::size_t port;
};

void from_json( const json& j, auth& m_auth )
{
    j.at("local").get_to(m_auth.local);
    j.at("server_pub_key").get_to(m_auth.server_pub_key);
    j.at("client_login").get_to(m_auth.client_login);
    j.at("port").get_to(m_auth.port);
}

    
}