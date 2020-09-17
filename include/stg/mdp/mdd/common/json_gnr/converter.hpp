#pragma once

#include <iostream>
#include <string>

#include <arpa/inet.h>

namespace stg::mdp::mdd::common::converter
{

class converter
{
public:
    converter() {}
    ~converter() {}

    static const std::string ipv_4(std::string& ip_addr)
    {
        static unsigned long ip_number = inet_addr(ip_addr.c_str());

        ip_number = ntohl(ip_number);
        ip_number += 1;
        ip_number = htonl(ip_number);

        ip_in_addr.s_addr = ip_number;

        return ( inet_ntoa(ip_in_addr) );
    }

protected:

private:
    static in_addr_t ip_number;
    static struct in_addr ip_in_addr;
};

in_addr_t converter::ip_number = inet_addr("233.46.176.0");
struct in_addr converter::ip_in_addr;

}
