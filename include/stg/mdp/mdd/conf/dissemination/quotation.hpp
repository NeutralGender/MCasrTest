#pragma once

#include <iostream>
#include <string>
#include <fstream>
#include <exception>

#include <stg/ai/ai.hpp>
#include <stg/mdp/mdd/conf/dissemination/types.hpp>

namespace stg::mdp::mdd::conf::dissemination
{

class diss_quota_conf
{
public:
    diss_quota_conf(const json& j):
                    quota( j["dissemination"]["quotation"] )
    {}

    ~diss_quota_conf()
    {}

    const struct dissemination_t::quotation_t quota;

};


}