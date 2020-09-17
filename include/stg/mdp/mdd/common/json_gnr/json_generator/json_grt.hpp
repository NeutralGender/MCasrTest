#pragma once

#include <stg/mdp/mdd/common/json_gnr/json_generator/json_types/json_element.hpp>
#include <stg/mdp/mdd/common/json_gnr/json_generator/json_types/array.hpp>
#include <stg/mdp/mdd/common/json_gnr/json_generator/json_types/number.hpp>
#include <stg/mdp/mdd/common/json_gnr/json_generator/json_types/object.hpp>
#include <stg/mdp/mdd/common/json_gnr/json_generator/json_types/string.hpp>
#include <stg/mdp/mdd/common/json_gnr/json_generator/json_types/empty_object.hpp>

#include <stg/mdp/mdd/common/json_gnr/converter.hpp>
#include <stg/mdp/mdd/common/json_gnr/writer/writer.hpp>

#include <string>

namespace stg::mdp::mdd::common::json_gnr::gnr
{

using stg::mdp::mdd::common::json_gnr::types::empty_object;
using stg::mdp::mdd::common::json_gnr::types::jarray;
using stg::mdp::mdd::common::json_gnr::types::jobject;
using stg::mdp::mdd::common::json_gnr::types::json_element;
using stg::mdp::mdd::common::converter::converter;
using stg::mdp::mdd::common::writer::writer_t;

class json_grt
{
public:
    json_grt( const std::string& local, const std::size_t port ):
            local(local),
            port(port)
    {}
    
    virtual ~json_grt() {}

    void build_config(const std::size_t count, std::string& ip_addr)
    { 
        empty_object start('\n');

        jobject root("production", '\n');
        
        jarray arr("A", '\n');

        empty_object empty('\n');

        arr.make_string("source", local, ',')
           .make_number("mask", 25, ',')
           .make_string("country", "US", ',')
           .make_string("city", "NY", ',');

        jarray ch("channel", '\n');

        for(std::size_t i = 0; i < ( count - 1 ); ++i)
        {
            ch.add_channel("A-CD", "0", converter::ipv_4(ip_addr), port, ',');
        }

        ch.add_channel("A-CD", "0", converter::ipv_4(ip_addr), port, '\n');

    }

    const std::string data()
    { return json_element::config.str(); }

    void write()
    { writer_t::write(json_element::config.str()); }

protected:

private:
    static constexpr char start[] = "{";
    static constexpr char end[] = "}";
    const std::string local;
    const std::size_t port;
};

}