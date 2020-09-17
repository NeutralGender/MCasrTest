#pragma once

#include <stg/mdp/mdd/net/unix/unix_socket.hpp>

#include <string>

namespace stg::mdp::mdd::net::s_unix::client
{

class client_socket: 
  public unix_socket
{    
public:
    client_socket(const std::string& s_addr_,
                  const short port_):
          unix_socket(s_addr_, port_) 
    {}

    virtual ~client_socket() 
    {}

    virtual int receive(std::string& data) = 0;

    virtual int send(const std::string& data)  = 0;
};

} // namespace stg::mdp::mdd::net::s_unix::client
