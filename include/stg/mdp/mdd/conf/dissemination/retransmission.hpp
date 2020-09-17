#pragma once

#include <iostream>
#include <string>
#include <fstream>
#include <exception>

#include <stg/ai/ai.hpp>
#include <stg/mdp/mdd/conf/dissemination/types.hpp>

namespace stg::mdp::mdd::conf::dissemination
{

class diss_retransm_conf
{    
public:
    diss_retransm_conf(const json& j):
                       retr( j["dissemination"]["retransmission"] )
    {}

    ~diss_retransm_conf()
    {}

    const struct dissemination_t::retransmission_t retr;

};


}