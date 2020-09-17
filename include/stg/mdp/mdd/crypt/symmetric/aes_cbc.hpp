#pragma once

#include <stg/mdp/mdd/crypt/symmetric/aes_iv.hpp>

#include <vector>
#include <string>

#include <crypto++/aes.h>
#include <crypto++/cryptlib.h> // Exception
#include <crypto++/modes.h> // CBC_Mode
#include <crypto++/filters.h> // StreamTransformationFilter; StringSink

namespace stg::mdp::mdd::crypt::symmetric
{

typedef unsigned char byte;

using CryptoPP::AES;
using CryptoPP::CBC_Mode;
using CryptoPP::StringSource;
using CryptoPP::StreamTransformationFilter;

class aes_cbc: 
 public aes_iv
{
public:
    aes_cbc(const std::size_t key_length_,
            const std::size_t iv_length_):
         aes_iv( key_length_, iv_length_ ) 
    {}

    virtual ~aes_cbc() 
    {}

    void set_key_iv_length(std::vector<byte>& key, std::vector<byte>& iv)
    {
        try
        {
            key.resize(key_length);
            iv.resize(iv_length);
        }
        
        catch(const std::exception& e)
        { std::cerr << e.what() << '\n'; }
    }

    virtual void encrypt(const std::vector<byte>& key, 
                         const std::vector<byte>& iv,
                         const std::string& plaintext, 
                         std::string& encoded) override
    {
        try
        {
            CBC_Mode<AES>::Encryption encr;
            encr.SetKeyWithIV( key.data(), key.size(), iv.data() );
            
            StringSource ss( plaintext,
                             true,
                             new StreamTransformationFilter // adding padding as required
                             (                             //  CBC must be padded
                                encr,
                                new CryptoPP::StringSink(encoded)
                             )
                            );
        }
        catch(const CryptoPP::Exception& e)
        {
            std::cerr << e.what() << '\n';
        }
    }

    virtual void decrypt(const std::vector<byte>& key, 
                         const std::vector<byte>& iv,
                         const std::string& encoded, 
                         std::string& plaintext) override
    {
        try
        {
            plaintext.resize(key_length);

            CBC_Mode<AES>::Decryption decr;
            decr.SetKeyWithIV( key.data(), key.size(), iv.data() );
            
            StringSource ss(encoded,
                            true,
                            new StreamTransformationFilter // removing padding as required
                            (                             //  CBC must be padded
                                decr,
                                new CryptoPP::StringSink(plaintext)
                            )
                            );
                
        }
        catch(const CryptoPP::Exception& e)
        {
            std::cerr << e.what() << '\n';
        }
    }

    virtual void decrypt(const std::vector<byte>& key, 
                         const std::vector<byte>& iv,
                         const std::string& encoded, 
                         std::vector< byte >& plaintext) override
    {
        try
        {
            plaintext.resize(key_length);

            CBC_Mode<AES>::Decryption decr;
            decr.SetKeyWithIV( key.data(), key.size(), iv.data() );

            CryptoPP::ArraySource ss(encoded,
                                     true,
                                     new StreamTransformationFilter // removing padding as required
                                     (                             //  CBC must be padded
                                        decr,
                                        new CryptoPP::ArraySink(plaintext.data(),
                                                                plaintext.size())
                                     )
                                    );   
                
        }
        catch(const CryptoPP::Exception& e)
        {
            std::cerr << e.what() << '\n';
        }
    }

};

} // namespace stg::mdp::mdd::crypt::symmetric