#pragma once

#include <crypto++/osrng.h> // AutoSeededRandomPool

#include <vector>
#include <string>

namespace stg::mdp::mdd::crypt::symmetric
{

typedef unsigned char byte;

class symmetric
{
public:
    symmetric(const std::size_t key_length_):
                      key_length(key_length_) 
    {}

    virtual ~symmetric() 
    {}

    void gen_key(std::vector<byte>& key)
    {
        try
        { prng.GenerateBlock( key.data(), key_length ); }

        catch(const CryptoPP::Exception& e)
        { std::cerr << e.what() << '\n'; }
    }

    virtual void gen_iv(std::vector<byte>& iv) 
    {}

    virtual void encrypt(const std::vector<byte>& key, 
                         const std::vector<byte>& iv,
                         const std::string& plaintext, 
                         std::string& encoded) = 0;
                         
    virtual void decrypt(const std::vector<byte>& key, 
                         const std::vector<byte>& iv,
                         const std::string& encoded, 
                         std::string& plaintext) = 0;

protected:
    const std::size_t key_length;
    CryptoPP::AutoSeededRandomPool prng;
};


} // namespace stg::mdp::mdd::crypt::symmetric