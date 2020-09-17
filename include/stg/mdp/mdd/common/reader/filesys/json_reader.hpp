#pragma once

#include <stg/ai/ai.hpp>

#include <string>
#include <fstream>
#include <iterator>

namespace stg::mdp::mdd::common::fs
{


class json_reader
{
public:
    json_reader()
    {}

    ~json_reader()
    { in.close(); }

    /**
     * @brief Open file
     * 
     * @param path file path
     * @return true if file was opened
     * @return false if cannot open file
     */
    bool open(const std::string& path);

    /**
     * @brief Read json file
     * 
     * @return const json& 
     */
    const json& read();

    /**
     * @brief Get json object
     * 
     * @return const std::string&
     */
    const json& get_data();

protected:

private:
    std::ifstream in;
    std::string data;
    json j;
};

[[nodiscard]]
bool json_reader::open(const std::string& path)
{
    in.open(path, std::ios_base::in);
    return ( not in.is_open() ) ? false : true;
}

const json& json_reader::read()
{
    std::copy( std::istream_iterator<char>{in}, std::istream_iterator<char>{}, std::back_inserter(data) );
    j = json::parse( data );

    return j;
}

const json& json_reader::get_data()
{ return (j); }

}