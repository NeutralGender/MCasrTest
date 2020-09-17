#pragma once

#include <stg/mdp/mdd/net/unix/client/client_socket.hpp>
#include <stg/ai/log.hpp>

#include <string>

#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

namespace stg::mdp::mdd::net::s_unix::client
{

#define LOG_TAG "[stg.ai.test.xring] "

class tcp_client: 
public client_socket
{    
public:
    tcp_client(const std::string& s_addr_, const short port_):
                                 client_socket(s_addr_, port_) 
    {}

    virtual ~tcp_client() 
    {}

    int connect_socket() 
    {
        if( ( connect( sockfd, (struct sockaddr*)&sock_sockaddr, sizeof(sock_sockaddr) ) ) < 0 )
        {
            LOG_CON(LOG_TAG "Tcp client connect_socket: connect() return -1; errno: %d\n", errno);
            return ( -1 );
        }
        return ( 0 );
    }

    virtual int receive(std::string& receive_data) override
    {
        int read_count = 0;

        receive_data.reserve( 1500 ); // current mtu on virtualbox 
        if( ( read_count = read( sockfd, receive_data.data(), receive_data.size() ) ) < 0 )
        {
            LOG_CON(LOG_TAG "Tcp client receive: read() return -1, errno: %d\n", errno);
            return ( -1 );
        }
        receive_data.shrink_to_fit();

        return ( read_count );
    }

    virtual int send(const std::string& send_data) override
    {
        int write_count = 0;

        if( ( write_count = write( sockfd, send_data.c_str(), send_data.size() ) ) < 0 )
        {
            LOG_CON(LOG_TAG "Tcp client send: write() return -1, errno: %d\n", errno);
            return ( -1 );
        }

        return ( write_count );
    }

};

} // namespace stg::mdp::mdd::net::s_unix::client