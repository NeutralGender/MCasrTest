#pragma once

#include <string>

#include <stg/mdp/mdd/net/unix/server/server_socket.hpp>
#include <stg/ai/log.hpp>

#include <sys/types.h>
#include <sys/socket.h>

namespace stg::mdp::mdd::net::s_unix::server
{

#define LOG_TAG "[stg.ai.test.xring] "

class tcp_server:
public server_socket
{
public:
    tcp_server(const std::string& s_addr_, const short port_):
                                  server_socket(s_addr_,port_) 
    {}

    virtual ~tcp_server() 
    {}

    int listen_socket(const std::size_t backlog) 
    {
        if( ( listen( sockfd, backlog ) ) < 0 )
        {
            LOG_CON(LOG_TAG "Tcp server listen_socket: listen return -1; errno: %d\n", errno);
            return ( -1 );
        }
        return 0;
    }

    int accept_socket() 
    {
        int new_conn = 0;
        if( ( new_conn = accept( sockfd, (struct sockaddr*)&sock_sockaddr, &len ) ) < 0 )
        {
            LOG_CON(LOG_TAG "Tcp server accept_socket: accept return -1; errno: &d\n", errno);
            return ( -1 );
        }

        return ( new_conn );
    }

    virtual int receive(const std::size_t new_conn, 
                        std::string& receive_data) override
    {
        int read_count = 0;

        receive_data.reserve( 1500 ); // current mtu on virtualbox 
        if( ( read_count = read( sockfd, receive_data.data(), receive_data.size() ) ) < 0 )
        {
            LOG_CON(LOG_TAG "Tcp server receive: read return -1' errno: %d\n", errno);
            return ( -1 );
        }
        receive_data.shrink_to_fit();

        return ( read_count );
    }

    virtual int send(const std::string& send_data) override
    {
        int write_count = 0;
        if( ( write_count = write( sockfd, send_data.c_str(), send_data.size() )  < 0 ) )
        {
            LOG_CON(LOG_TAG "Tcp server send: write return -1' errno: %d\n", errno);
            return ( -1 );
        }

        return ( write_count );
    }

};


} // namespace stg::mdp::mdd::net::s_unix::server