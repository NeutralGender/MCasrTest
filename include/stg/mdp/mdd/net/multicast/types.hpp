#pragma once

#include <cstddef>
#include <sys/types.h>
#include <sys/socket.h>

namespace stg::mdp::mdd::net::multicast
{
    enum class mcast_type
    {
        ANY = 0, //!< Any Source Multicast
        SSM      //!< Source Specific Multicast
    };

} // namespace stg::mdp::mdd::net::multicast
