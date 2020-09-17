#pragma once

#include <stg/mdp/mdd/db/db_driver.hpp>
#include <stg/mdp/mdd/db/redis/redis_connection.hpp>
#include <stg/mdp/mdd/db/redis/redis_commands.hpp>

#include <stg/mdp/mdd/serialize/record.hpp>

#include <array>
#include <map>
#include <string>
#include <utility>

namespace stg::mdp::mdd::db::redis
{

using stg::mdp::mdd::serialize::record;

class redis_driver: 
 public db_driver_t
{
public:
    redis_driver(const std::string& hostname_,
                 const std::size_t port_):
              rconnection(hostname_,port_) 
    {}

    virtual ~redis_driver() 
    {}

    virtual void connect_db() override
    { rconnection.connect_db(); }

    void ssl_connection(const std::string& CA_crt, 
                        const std::string& client_cert,
                        const std::string& client_private_key,
                        const std::string& server_to_request)
    { 
        rconnection.secure_connect(CA_crt,
                                   client_cert,
                                   client_private_key,
                                   server_to_request ); 
    }

    virtual void get_all_dataR(std::vector< record >& records) override
    { r_command.get_all_data( rconnection, records ); }

    virtual void authentication(const std::string& key) override
    { r_command.auth(rconnection, key); }

    virtual std::size_t size() override
    { return ( r_command.size( rconnection ) ); }

private:
    redis_connection rconnection;
    redis_commands r_command;

};

} // namespace stg::mdp::mdd::db::redis
