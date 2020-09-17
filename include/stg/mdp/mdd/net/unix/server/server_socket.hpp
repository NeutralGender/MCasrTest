#pragma once

#include <stg/mdp/mdd/net/unix/unix_socket.hpp>
#include <stg/ai/log.hpp>

#include <string>

#include <sys/types.h>
#include <sys/socket.h>

namespace stg::mdp::mdd::net::s_unix::server
{

#define LOG_TAG "[stg.ai.test.xring] "

class server_socket: 
  public unix_socket
{
public:
    server_socket(const std::string& s_addr_, const short port_):
                                      unix_socket(s_addr_, port_) 
    {}

    virtual ~server_socket() 
    {}

    void bind_socket()
    {
        if( ( bind( sockfd, (struct sockaddr*)&sock_sockaddr, sizeof(sockaddr) ) < 0 ) )
        {
            printf( "Server bind return -1; errno: %d\n", errno );
            LOG_CON(LOG_TAG "Server bind return -1; errno: %d\n", errno);
            return;
        }
    }

    virtual int receive(const std::size_t new_conn, std::string& receive_data) {}

    virtual int send(const std::size_t new_conn, const std::string& send_data) {}

    virtual int send(const std::string& send_data) {}

    virtual int receive(std::string& receive_data) {}

protected:
    socklen_t len = sizeof( sock_sockaddr );

};


} // namespace stg::mdp::mdd::net::s_unix::server