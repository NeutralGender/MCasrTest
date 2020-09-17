#pragma once

#include <iostream>
#include <string>
#include <fstream>
#include <exception>

#include <stg/mdp/mdd/conf/auth_conf/types.hpp>
#include <stg/ai/ai.hpp>

namespace stg::mdp::mdd::conf::auth_conf
{

class auth_conf
{
public:
    auth_conf(const json& j):
              m_auth( j["auth"] )
    {}

    ~auth_conf()
    {}

    const struct auth m_auth;

};

    
}