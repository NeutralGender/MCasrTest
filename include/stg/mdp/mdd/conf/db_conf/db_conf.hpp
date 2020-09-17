#pragma once

#include <iostream>
#include <string>
#include <fstream>
#include <exception>

#include <stg/mdp/mdd/conf/db_conf/types.hpp>
#include <stg/ai/ai.hpp>

namespace stg::mdp::mdd::conf::db_conf
{

class db_conf
{
public:
    db_conf(const json& j):
            redis( j["db"]["redis"] ),
            psql( j["db"]["psql"] ),
            redis_ssl( j["secure_connection"]["redis_secure"] )
    {}

    ~db_conf()
    {}

    const struct db_endpoint_conf::redis redis;
    const struct db_endpoint_conf::psql psql;
    const struct secure_connection_conf::redis_ssl redis_ssl;
};

}