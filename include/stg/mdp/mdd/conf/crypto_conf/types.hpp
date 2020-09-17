#pragma once

#include <cstddef>

#include <stg/ai/ai.hpp>

namespace stg::mdp::mdd::conf::crypto_conf
{

struct crypto
{
    struct asymmetric
    {
        struct RSA
        {
            struct rsa_server
            { std::size_t key_length; };

            struct rsa_client
            { std::size_t key_length; };
        };
    };

    struct symmetric
    {
        struct AES
        {
            std::size_t key_length;
            std::size_t iv;
        };
    };
};

void from_json( const json& j, crypto::asymmetric::RSA::rsa_server& serv )
{ j.at("key_length").get_to(serv.key_length); }

void from_json( const json& j, crypto::asymmetric::RSA::rsa_client& cli )
{ j.at("key_length").get_to(cli.key_length); }

void from_json( const json& j, crypto::symmetric::AES& aes )
{ 
    j.at("key_len").get_to(aes.key_length); 
    j.at("iv_len").get_to(aes.iv); 
}

}