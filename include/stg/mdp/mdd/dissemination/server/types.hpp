#pragma once

#include <cstddef>
#include <string>

namespace stg::mdp::mdd::server
{

using addr_data = std::multimap<std::string, std::string>;

struct test_diss_mock
{
    u_int32_t seq;
    u_int32_t caplen;
    u_char* value;
};

/**
 * @brief Struct for test Multicast dissemination
 */
struct diss_mock
{
    std::size_t seq;
    std::string tag;
    std::string value;
};

}