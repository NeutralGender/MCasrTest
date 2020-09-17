#pragma once

#include <stg/mdp/mdd/net/multicast/mcast_socket.hpp>
#include <stg/mdp/mdd/conf/dissemination/types.hpp>
#include <stg/ai/log.hpp>

#include <iostream>
#include <string.h>
#include <exception>

#include <sys/socket.h>
#include <sys/types.h>
#include <string.h>

namespace stg::mdp::mdd::net::multicast
{

using stg::mdp::mdd::conf::dissemination::production;

class mcast_sender:
public mcast_socket
{
public:
    /**
     * @brief Construct a new multicast send object
     * 
     * @param local_addr_ local IPv4 address which will send
     * @param group_addr_ multicast group IPv4 address
     * @param port_ sender port
     */
    mcast_sender (const std::string& local_addr_,
                  const std::string& group_addr_,
                  const std::size_t port_):
                  local_addr(local_addr_),
                  group_addr(group_addr_),
                  mcast_socket(port_) 
    {}

    mcast_sender(const std::string& src, const production& channel):
                 local_addr(src),
                 group_addr(channel.group),
                 mcast_socket(channel.port) 
    {}

    virtual ~mcast_sender() 
    {}


    virtual void init_socket() override
    {        
        bzero(&group,sizeof(group));
        group.sin_family = AF_INET;
        group.sin_port = htons(port);
        group.sin_addr.s_addr = inet_addr(group_addr.c_str());
    }

    /**
     * @brief Set socket settings: loop and interface
     * 
     * @return true if set_loop() and set_out_interface both returns true
     * @return false if set_loop() and set_out_interface both returns true
     */
    [[nodiscard]]
    virtual bool socket_settings() override
    { return ( set_loop() && set_out_interface() && set_ttl() ) == false ?  false : true; }

    int send(const std::string& send_data)
    {
        #define LOG_TAG "[stg.ai.test.xring] "
        
        int send_count = 0;
        if( ( send_count = sendto( sockfd, send_data.data(),
                                   send_data.size(), 0,
                                   (struct sockaddr*)&group,
                                   sizeof(group) )) < 0 )
        {
            std::cout << inet_ntoa(group.sin_addr) << std::endl;
            static std::size_t i = 0;
            i++;
            std::cout << i << std::endl;
            LOG_CON(LOG_TAG "mcast_sender send: sendto return -1; errno: %d\n", errno);
            //std::cin.get();
            return ( -1 );
        }
        
        return ( send_count );

        #undef LOG_TAG
    }

private:
    struct in_addr local;
    struct sockaddr_in group;

private:
/*
    void set_loop()
    {
        #define LOG_TAG "[stg.ai.test.xring] "

        try
        {
            u_char loop = 1;
            if( ( setsockopt( sockfd, IPPROTO_IP, IP_MULTICAST_LOOP,
                              (&loop), sizeof(loop) )  ) < 0 )
                throw std::runtime_error("Cannot set REUSEADDR setsockopt()");
        }
        catch(const std::runtime_error& r)
        { LOG_CON(LOG_TAG "%s : %d\n", r.what(), errno); } 

        #undef LOG_TAG
    }
*/

    [[nodiscard]] bool set_loop()
    {
    #define LOG_TAG "[stg.ai.test.xring] "

        u_char loop = 1;
        if( ( setsockopt( sockfd, IPPROTO_IP, IP_MULTICAST_LOOP, (&loop), sizeof(loop) )  ) < 0 )
        {
            LOG_CON(LOG_TAG "%s : %d\n", "Cannot set REUSEADDR setsockopt()", errno);
            return false;
        }

        return true;

    #undef LOG_TAG
    }

/*
    void set_out_interface()
    {
        #define LOG_TAG "[stg.ai.test.xring] "

        try
        {
            local.s_addr = inet_addr(local_addr.c_str());

            if( ( setsockopt( sockfd, IPPROTO_IP, IP_MULTICAST_IF,
                             &local, sizeof(local) )  ) < 0 )
                throw std::runtime_error("Cannot SetLocalInterface");
        }
        catch(const std::runtime_error& r)
        { LOG_CON(LOG_TAG "%s : %d\n", r.what(), errno); }

        #undef LOG_TAG
    }
*/

    [[nodiscard]] bool set_out_interface()
    {
    #define LOG_TAG "[stg.ai.test.xring] "

        local.s_addr = inet_addr(local_addr.c_str());
        if( ( setsockopt( sockfd, IPPROTO_IP, IP_MULTICAST_IF, &local, sizeof(local) )  ) < 0 )
        {
            LOG_CON(LOG_TAG "%s : %d\n", "Cannot SetLocalInterface", errno);
            return false;
        }

        return true;

    #undef LOG_TAG
    }

    [[nodiscard]] bool set_ttl()
    {
    #define LOG_TAG "[stg.ai.test.xring] "

        u_char ttl = 64; // Default Linux ttl value
        if( ( setsockopt( sockfd, IPPROTO_IP, IP_MULTICAST_TTL, &ttl, sizeof(ttl) ) ) < 0 )
        {
            LOG_CON(LOG_TAG "%s : %d\n", "Cannot SetMulticastTTL", errno);
            return false;
        }

        return true;

    #undef LOG_TAG
    }

private:
    const std::string& group_addr;
    const std::string& local_addr;

};


} // namespace stg::mdp::mdd::net::multicast