#pragma once

#include <stg/mdp/mdd/common/json_gnr/json_generator/json_types/json_element.hpp>
#include <stg/mdp/mdd/common/json_gnr/json_generator/json_types/string.hpp>
#include <stg/mdp/mdd/common/json_gnr/json_generator/json_types/number.hpp>

namespace stg::mdp::mdd::common::json_gnr::types
{

class empty_object : public json_element
{
private:
    using self = empty_object;
public:
    empty_object(const char coma) : json_element(start, end, coma)
    {}

    virtual ~empty_object() {}

    self& make_string(const std::string& field, const std::string& value, const char coma)
    { 
        jstring str(field, value, coma);
        return *this;
    }

    self& make_number(const std::string& field, const std::size_t value, const char coma)
    {  
        jnumber num(field, value, coma);
        return *this;
    }

private:
    static constexpr char start[] = "{";
    static constexpr char end[] = "}";
};

}
