#pragma once

#include <iostream>
#include <string>
#include <exception>

#include <hiredis/hiredis.h>
#include <hiredis/hiredis_ssl.h>

namespace stg::mdp::mdd::db::redis
{

class redis_connection
{
public:
   redis_connection(const std::string& hostname_,
                    const std::size_t port_):
                          hostname(hostname_),
                                  port(port_) 
    {}

   ~redis_connection()
   { redisFree(rconnect); }

    redisContext* connection()
    { return ( rconnect ); }

    void connect_db()
    {
        try
        {
            rconnect = redisConnect(hostname.c_str(), port);

            if(rconnect == NULL || rconnect->err)
                throw std::runtime_error("No Redis Connection\n");
            else // TODO: remove after tests
                std::cout << "Redis Connected!\n"; // Temp
        
        }
        catch(const std::runtime_error& e)
        { std::cerr << e.what() << '\n'; }
    }

    int secure_connect(const std::string& CA_crt, 
                       const std::string& client_cert,
                       const std::string& client_private_key,
                       const std::string& server_to_request)
    {
        //TODO(Gennady Shibaev): add flags: -lssl -lcrypto if we will use secure connection
        try
        {
            if( (redisSecureConnection( rconnect, 
                                        CA_crt.c_str(), 
                                        client_cert.c_str(), 
                                        client_private_key.c_str(),
                                        server_to_request.c_str() )) != REDIS_OK )
                {
                    throw std::runtime_error( "redis_connection secure_connect not return REDIS_OK" );
                    return ( -1 );
                }
        }
        catch(const std::runtime_error& run)
        { std::cerr << run.what() << '\n'; }
        
        return ( 0 );
    }

    /**
     * @brief Must set ALL class as friend, because of predecloration RedisConnection
     * @relates redis_commands
     * association relation:
     *  - connection not belongs to one object 
     *  - connection can exists without commenad object
     *  - connection doesn`t know about command object
     * 
     */ //TODO(Gennady Shibaev): Maybe remove and change architecture
    friend class redis_commands; // Must set ALL class as friend,
                               // because of predecloration redis_connection
                               // 

private:
    const std::string hostname;
    const std::size_t port;

    redisContext* rconnect;

};


} // namespace stg::mdp::mdd::db::redis
