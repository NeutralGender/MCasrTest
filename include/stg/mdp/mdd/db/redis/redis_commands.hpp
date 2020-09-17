#pragma once

#include <iostream>
#include <string>
#include <vector>
#include <cstring>

#include <hiredis/hiredis.h>
#include <stg/mdp/mdd/serialize/record.hpp>

namespace stg::mdp::mdd::db::redis
{

using stg::mdp::mdd::serialize::record;

class redis_commands
{
public:
    redis_commands() 
    {}

    ~redis_commands() 
    {}
            
    void get_all_data(redis_connection& rconnection, std::vector<record>& records);

    void auth(redis_connection& rconnection, 
              const std::string& pwd);

    std::size_t size(redis_connection& rconnection);

private:
    redisReply* reply;

};

void redis_commands::auth(redis_connection& rconnection, const std::string& pwd)
{
    try
    {
        if( ( reply = static_cast<redisReply*>(redisCommand( rconnection.rconnect, 
                                                             "AUTH %s", 
                                                             pwd.c_str( ) )
                                              ) )->type == REDIS_REPLY_ERROR )
        throw std::runtime_error( "Redis Authentication failed!\n" );

        printf( "Authentication complete\n" );

    }
    catch(const std::runtime_error& run)
    {
        std::cerr << run.what() << '\n';
    }
}

void redis_commands::get_all_data(redis_connection& rconnection, std::vector<record>& vrecord)
{
    vrecord.clear();
    
    reply = static_cast<redisReply*>(redisCommand(rconnection.rconnect, "KEYS *"));

    redisReply *temp = nullptr;
    for( size_t i = 0; i < reply->elements; i++ )
    {
        temp = static_cast<redisReply*>(redisCommand(rconnection.rconnect,
                                                     "HGETALL %b", &reply->element[i]->str[0],//8)
                                                     (size_t) strlen(reply->element[i]->str))
                                        );

        for(size_t j = 0; j < temp->elements; j = j+2)
        {
            vrecord.push_back( record( std::string{reply->element[i]->str,
                                                    strlen(reply->element[i]->str)},
                                       std::string{ temp->element[j]->str, 
                                                    strlen(temp->element[j]->str) },
                                       std::string{ temp->element[j+1]->str,
                                                    strlen(temp->element[j+1]->str) }
                                     ) );
        }


        freeReplyObject(temp);
    }

}

std::size_t redis_commands::size( redis_connection& rconnection )
{
    reply = static_cast<redisReply*>( redisCommand( rconnection.rconnect, "dbsize" ) );

    return ( 0 < reply->integer ) ? reply->integer : 0;
}


} // namespace stg::mdp::mdd::db::redis