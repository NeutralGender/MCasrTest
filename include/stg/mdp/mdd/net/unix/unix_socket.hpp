#pragma once

#include <iostream>
#include <string>

#include <net/socket.hpp>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>

#include <stg/ai/log.hpp>

namespace stg::mdp::mdd::net::s_unix
{

#define LOG_TAG "[stg.ai.test.xring] "

/**
 * @brief Abstract class for native unix sockets
 */
class unix_socket:
   public socket_t
{
public:
    unix_socket(const std::string& s_addr_,
                const short port_):
                    s_addr(s_addr_), 
                        port(port_) 
    {}

    virtual ~unix_socket() 
    {}

    virtual void init_socket() override
    {
        bzero( &sock_sockaddr, sizeof(sock_sockaddr) );
        sock_sockaddr.sin_family = AF_INET;
        sock_sockaddr.sin_port = htons(port);
        sock_sockaddr.sin_addr.s_addr = inet_addr( s_addr.c_str() );
    }
protected:
    const std::string s_addr;
    const std::size_t port;
    struct sockaddr_in sock_sockaddr;
};

} // namespace stg::mdp::mdd::net::s_unix