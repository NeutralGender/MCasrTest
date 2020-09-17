#pragma once

#include <iostream>
#include <string>
#include <fstream>
#include <exception>

#include <stg/mdp/mdd/conf/crypto_conf/types.hpp>
#include <stg/ai/ai.hpp>

namespace stg::mdp::mdd::conf::crypto_conf
{

class crypto_conf
{
public:
    crypto_conf(const json& j):
                aes( j["crypto"]["symmetric"]["AES"] ),
                rsa_server( j["crypto"]["asymmetric"]["RSA"]["server"] ),
                rsa_client( j["crypto"]["asymmetric"]["RSA"]["client"] )
    {}

    ~crypto_conf()
    {}

    const struct crypto::symmetric::AES aes;
    const struct crypto::asymmetric::RSA::rsa_server rsa_server;
    const struct crypto::asymmetric::RSA::rsa_client rsa_client;
};


}