#pragma once

#include <vector>
#include <string>

#include <stg/mdp/mdd/crypt/symmetric/aes_iv.hpp>
#include <stg/mdp/mdd/crypt/asymmetric/asymmetric.hpp>
#include <stg/mdp/mdd/crypt/hash/hash.hpp>
#include <stg/mdp/mdd/net/unix/client/tcp_client.hpp>

namespace stg::mdp::mdd::auth
{

typedef unsigned char byte;

using stg::mdp::mdd::net::s_unix::client::tcp_client;
using stg::mdp::mdd::crypt::asymmetric::asymmetric;
using stg::mdp::mdd::crypt::symmetric::aes_iv;
using stg::mdp::mdd::crypt::hash::hash;

class auth_t
{
public:
    auth_t(const std::string server_pub_path_,
           const std::string client_login_):
           server_pub_path(server_pub_path_),
                 client_login(client_login_) 
    {}

    virtual ~auth_t() 
    {}

    virtual void auth_client(aes_iv& AES, asymmetric& server, asymmetric& client, hash& sha,
                             tcp_client& cli_socket,
                             std::vector<byte>& permanent_key,
                             std::vector<byte>& permanent_iv,
                             std::vector<byte>& AES_Ticket) = 0;
protected:
    const std::string server_pub_path;
    const std::string client_login;
};

} // namespace stg::mdp::mdd::auth