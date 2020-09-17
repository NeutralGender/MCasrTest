#pragma once

#include <iostream>
#include <string>
#include <fstream>
#include <exception>

#include <stg/ai/ai.hpp>
#include <stg/mdp/mdd/conf/dissemination/types.hpp>

namespace stg::mdp::mdd::conf::dissemination
{

class diss_prod_conf
{
public:
    diss_prod_conf(const json& j):
                   prod_a( j["production"]["A"] )//,
                   //prod_b( j["production"]["B"] )
    {}

    ~diss_prod_conf()
    {}

    const struct dissemination_t::production_t prod_a;
    //const struct dissemination_t::production_t prod_b;
};


}