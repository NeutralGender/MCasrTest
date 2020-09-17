#pragma once

#include <string>
#include <map>

namespace stg::mdp::mdd::common::reader
{

/**
 * @brief Reader Interface
 * 
 */
class read_api
{
public:
    // Maybe you have better name
    using addr_data = std::multimap<std::string, std::string>;

    virtual ~read_api() {}

    /**
     * @brief Open file
     * 
     * @param open_name file path
     * @return true if file was opened
     * @return false if cannot open file
     */
    [[nodiscard]]
    virtual bool open( const std::string& open_name ) = 0;

    /**
     * @brief Read file
     * 
     * @param data_string container for <address:data> from file
     * @return void 
     */
    virtual std::tuple<u_int32_t, u_int8_t*> read() = 0;

    /**
     * @brief Get the data object
     */
    virtual addr_data& get_data() = 0;
};


}