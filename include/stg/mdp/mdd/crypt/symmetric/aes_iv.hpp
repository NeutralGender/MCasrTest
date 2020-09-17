#pragma once

#include <stg/mdp/mdd/crypt/symmetric/aes.hpp>

#include <vector>
#include <string>

namespace stg::mdp::mdd::crypt::symmetric
{

typedef unsigned char byte;

class aes_iv:
   public aes
{
public:
    aes_iv(const std::size_t key_length_,
           const std::size_t iv_length_): 
                         aes(key_length_),
                    iv_length(iv_length_) 
    {}

    virtual ~aes_iv() 
    {}

    virtual void gen_iv(std::vector<byte>& iv)
    {
        try
        { prng.GenerateBlock( iv.data(), iv_length); }

        catch(const CryptoPP::Exception& e)
        { std::cerr << e.what() << '\n'; }
    }

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

    virtual void decrypt(const std::vector<byte>& key,
                         const std::vector<byte>& iv,
                         const std::string& encoded,
                         std::vector< byte >& plaintext) 
    {}

protected:
    const size_t iv_length;
};

} // namespace stg::mdp::mdd::crypt::symmetric

