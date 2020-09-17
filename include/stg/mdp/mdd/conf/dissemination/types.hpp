#pragma once

#include <string>
#include <vector>

#include <json/json.hpp>
using json = nlohmann::json;

namespace stg::mdp::mdd::conf::dissemination
{

using json = nlohmann::json;

enum class country_type
{
    US = 0
};

enum class city_type
{
    NY = 0,
    Chicago
};

enum class feed_type
{
    A = 0,
    B
};

struct abstract
{
    std::string name;
    std::string tag;
    unsigned short port;
};

struct production : abstract
{
    std::string group;
};

struct retransmission : abstract
{
    std::string src;
};

struct quotation : abstract
{
    std::string group;
};

struct production_abstract
{
    std::vector< production > broadcast;

    std::string country;
    std::string city;
};

struct request_t
{
    std::vector < retransmission > broadcast;

    std::string country;
    std::string city;
};

struct channel_t : production_abstract
{
    std::string src;
    unsigned short mask;
};

struct test_t : production_abstract
{
    std::string src;
};

struct dissemination_t
{
    struct production_t
    {
        std::vector<channel_t> channels;

    } production;

    struct retransmission_t
    {
        std::vector<request_t> channels;

    } retransmission;

    struct quotation_t
    {
        std::vector<test_t> channels;
        
    } quotation;

};

void from_json( const json& j, production& prod )
{
    j.at( "name" ).get_to( prod.name );
    j.at( "tag" ).get_to( prod.tag );
    j.at( "group" ).get_to( prod.group );
    j.at( "port" ).get_to( prod.port );
}

void from_json( const json& j, retransmission& ret )
{
    j.at( "name" ).get_to( ret.name );
    j.at( "tag" ).get_to( ret.tag );
    j.at( "port" ).get_to( ret.port );
    j.at( "source" ).get_to( ret.src );
}

void from_json( const json& j, production_abstract& broadcst )
{
    for( auto& elem : j )
        broadcst.broadcast.push_back( elem );
}

void from_json( const json& j, request_t& broadcst )
{
    for( auto& elem : j )
        broadcst.broadcast.push_back( elem );
}

void from_json( const json& j, dissemination_t::production_t& prod )
{
    for( auto& elem : j )
    {
        prod.channels.push_back( elem["channel"] );
        prod.channels[ &elem - &j[0] ].src = elem["source"];
        prod.channels[ &elem - &j[0] ].mask = elem["mask"];
        prod.channels[ &elem - &j[0] ].country = elem["country"];
        prod.channels[ &elem - &j[0] ].city = elem["city"];
    }
}

void from_json( const json& j, dissemination_t::retransmission_t& ret )
{
    for( auto& elem : j )
    {
        ret.channels.push_back( elem["request"] );
        ret.channels[ &elem - &j[0] ].country = elem["country"];
        ret.channels[ &elem - &j[0] ].city = elem["city"];
    }
}

void from_json( const json& j, dissemination_t::quotation_t& ret )
{
    for( auto& elem : j )
    {
        ret.channels.push_back( elem["test_broadcast"] );
        ret.channels[ &elem - &j[0] ].country = elem["country"];
        ret.channels[ &elem - &j[0] ].city = elem["city"];
        ret.channels[ &elem - &j[0] ].src = elem["source"];
    }
}

}