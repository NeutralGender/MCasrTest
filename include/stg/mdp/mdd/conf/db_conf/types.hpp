#pragma once

#include <cstddef>
#include <string>

#include <stg/ai/ai.hpp>

namespace stg::mdp::mdd::conf::db_conf
{

struct db_endpoint_conf
{
    struct redis
    {
        std::string host;
        std::size_t port;
    };

    struct psql
    {
        std::string db;
        std::string usr;
        std::string passwd;
        std::string host;
        std::string port;
    };
};

struct secure_connection_conf
{
    struct redis_ssl
    {
        std::string passwd;
        std::string CA_cert;
        std::string client_cert;
        std::string client_private_key;
        std::string server_request;
    };
};


void from_json( const json& j, db_endpoint_conf::redis& m_redis )
{ 
    j.at("host").get_to(m_redis.host); 
    j.at("port").get_to(m_redis.port); 
}

void from_json( const json& j, db_endpoint_conf::psql& postgres )
{
    j.at("db").get_to(postgres.db);
    j.at("usr").get_to(postgres.usr);
    j.at("passwd").get_to(postgres.passwd);
    j.at("host").get_to(postgres.host);
    j.at("port").get_to(postgres.port);
}

void from_json( const json& j, secure_connection_conf::redis_ssl& m_redis )
{
    j.at("passwd").get_to(m_redis.passwd);
    j.at("CA_cert").get_to(m_redis.CA_cert);
    j.at("client_cert").get_to(m_redis.client_cert);
    j.at("client_private_key").get_to(m_redis.client_private_key);
    j.at("server_request").get_to(m_redis.server_request);
}


}