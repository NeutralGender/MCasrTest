#pragma once

#include <iostream>
#include <vector>
#include <string>
#include <stg/mdp/mdd/crypt/symmetric/symmetric.hpp>

/**
 * @brief Libraries for Crypto
 */
#include <crypto++/aes.h>
#include <crypto++/osrng.h>
#include <crypto++/hex.h>

namespace stg::mdp::mdd::crypt::symmetric
{

typedef unsigned char byte;

class aes:
public symmetric
{    
public:
    aes(const std::size_t key_length_): 
                 symmetric(key_length_) 
    {}

    virtual ~aes() 
    {}

    virtual void encrypt(const std::vector<byte>& key, 
                         const std::vector<byte>& iv,
                         const std::string& plaintext, 
                         std::string& encoded) override 
    {}
                         
    virtual void decrypt(const std::vector<byte>& key, 
                         const std::vector<byte>& iv,
                         const std::string& encoded, 
                         std::string& plaintext) override 
    {}
    
};


} // namespace stg::mdp::mdd::crypt::symmetric