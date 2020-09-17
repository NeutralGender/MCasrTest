#pragma once

#include <stg/mdp/mdd/crypt/hash/hash.hpp>

#include <string>
#include <vector>

#include <crypto++/cryptlib.h>
#include <crypto++/sha3.h>

namespace stg::mdp::mdd::crypt::hash
{

class sha3_256: 
    public hash
{
public:
    sha3_256() 
    {}

    virtual ~sha3_256() 
    {}

    virtual void add_data(const std::string& message) override
    { sha3.Update( (const byte*)message.data(), message.size() ); }

    virtual void set_digest_size(std::string& digest) override
    { digest.resize( sha3.DigestSize() ); }
    
    virtual void calculate_digest(std::string& digest) override
    { sha3.Final( (byte*)&digest[0] ); }

    virtual bool verify(const std::string& message, 
                        const std::string& digest) override
    {
        sha3.Update( (const byte*)message.data(), message.size() );
        return ( sha3.Verify( (const byte*)digest.data() ) );
    }

    virtual bool verify(const std::vector<byte>& message, 
                        const std::vector<byte>& digest) override
    {
        sha3.Update( (const byte*)message.data(), message.size() );
        return ( sha3.Verify( (const byte*)digest.data() ) );
    }

private:
    CryptoPP::SHA3_256 sha3;

};


} // namespace stg::mdp::mdd::crypt::hash 

