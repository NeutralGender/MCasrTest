#pragma once

#include <string>

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

#include <stg/mdp/mdd/net/unix/client/client_socket.hpp>
#include <stg/ai/log.hpp>

namespace stg::mdp::mdd::net::s_unix::client
{

#define LOG_TAG "[stg.ai.test.xring] "

class udp_client:
public client_socket
{
public:
    udp_client(const std::string& s_addr_, const short port_):
                                 client_socket(s_addr_, port_) 
    {}

    virtual ~udp_client() 
    {}

    virtual int receive(std::string& receive_data) override
    {
        int recv_count = 0;
        if( ( recv_count = recvfrom(sockfd, 
                                    receive_data.data(),
                                    receive_data.size(), 
                                    0,
                                    (struct sockaddr*)&sock_sockaddr,
                                    &len) ) < 0 )
        {
            LOG_CON(LOG_TAG "Udp client receive: recvfrom return -1, errno: %d\n", errno);
            return ( -1 );
        }
        
        receive_data.shrink_to_fit();

        return ( recv_count );
    }

    virtual int send(const std::string& send_data) override
    {
        int send_count = 0;
        if( ( send_count = sendto(sockfd, 
                                  send_data.data(),
                                  send_data.size(), 
                                  0,
                                  (struct sockaddr*)&sock_sockaddr,
                                  sizeof(sock_sockaddr)) ) < 0 )
        {
            LOG_CON(LOG_TAG "Udp client send: sendto return -1, errno: %d\n", errno);
            return ( -1 );
        }
        
        return ( send_count );
    }

private:
    socklen_t len = sizeof( sock_sockaddr );

};

} // namespace stg::mdp::mdd::net::s_unix::client