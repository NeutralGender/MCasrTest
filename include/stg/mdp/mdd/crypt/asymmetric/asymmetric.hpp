#pragma once

#include <iostream>
#include <vector>
#include <string>

namespace stg::mdp::mdd::crypt::asymmetric
{

typedef unsigned char byte;

class asymmetric
{  
public:
    asymmetric(const size_t& keysize_):
                      keysize(keysize_) 
    {}

    virtual ~asymmetric() 
    {}

    virtual void key_generation() = 0;

    virtual void load_pub_key_from_file(const std::string& pubkey_path) = 0;
    virtual void load_priv_key_from_file(const std::string& privkey_path) = 0;

    virtual void load_pub_key_from_string(const std::string& public_key_string) = 0;
    virtual void load_priv_key_from_string(const std::string& private_key_string) = 0;

    virtual void save_pub_key_to_file(const std::string& pubkey_string) = 0;
    virtual void save_priv_key_to_file(const std::string& privkey_string) = 0;

    virtual void save_pub_key_to_string(std::string& public_key_string) = 0;
    virtual void save_priv_key_to_string(std::string& private_key_string) = 0;

    virtual void encrypt(const std::string& plaintext, std::string& ciphertext) = 0;
    virtual void encrypt(const std::vector<byte>& plaintext, 
                         std::vector<byte>& ciphertext) = 0;

    virtual std::string encrypt(const std::string& plaintext) = 0;

    virtual void encrypt(const std::vector<byte>& plaintext, 
                         std::string& ciphertext) = 0;

    virtual void decrypt(const std::string& ciphertext, std::string& plaintext) = 0;
    virtual void decrypt(const std::vector<byte>& ciphertext, 
                         std::vector<byte>& plaintext) = 0;
    virtual std::string decrypt(const std::string& ciphertext) = 0;

    virtual void sign(const std::string& message, 
                      std::string& signature) = 0;
    virtual void sign(const std::vector< byte >& message, 
                      std::vector< byte >& signature) = 0;
    virtual std::string sign(const std::string& message) = 0;

    virtual std::vector<byte> sign(const std::vector<byte>& message) = 0;

    virtual std::vector< byte > verify(const std::vector< byte >& signature) = 0;
    
    virtual std::vector<byte> verify(const std::string& signature) = 0;

protected:
    const size_t& keysize;  

};

} // namespace stg::mdp::mdd::crypt::asymmetric