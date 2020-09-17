#pragma once

#include <iostream>
#include <string>
#include <vector>

namespace stg::mdp::mdd::crypt::hash
{

class hash
{
public:
    hash() 
    {}
    
    virtual ~hash() 
    {}

    virtual void add_data(const std::string& message) = 0; // Add Data  
    virtual void set_digest_size(std::string& digest) = 0;
    virtual void calculate_digest(std::string& digest) = 0;
    virtual bool verify(const std::string& message, 
                        const std::string& digest) = 0;

    virtual bool verify(const std::vector<byte>& message, 
                        const std::vector<byte>& digest) = 0;
};


} // namespace stg::mdp::mdd::crypt::hash
