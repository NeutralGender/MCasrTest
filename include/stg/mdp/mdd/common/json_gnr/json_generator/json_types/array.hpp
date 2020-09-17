#pragma once

#include <stg/mdp/mdd/common/json_gnr/json_generator/json_types/json_element.hpp>
#include <stg/mdp/mdd/common/json_gnr/json_generator/json_types/string.hpp>
#include <stg/mdp/mdd/common/json_gnr/json_generator/json_types/number.hpp>
#include <stg/mdp/mdd/common/json_gnr/json_generator/json_types/object.hpp>
#include <stg/mdp/mdd/common/json_gnr/json_generator/json_types/empty_object.hpp>

#include <string>

namespace stg::mdp::mdd::common::json_gnr::types
{

class jobject;

class jarray : public json_element
{
private:
    using self = jarray;
public:
    jarray(const std::string& root,
           const char coma):
           json_element(root, start, end, coma)
    {  }

    virtual ~jarray() {}

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

    self& add_channel(const std::string& name,
                      const std::string& tag,
                      const std::string& group,
                      const std::size_t port,
                      const char coma)
    {
        empty_object obj(coma);
        obj.make_string("name", name, ',');
        obj.make_string("tag", tag, ',');
        obj.make_string("group", group, ',');
        obj.make_number("port", port, '\n');

        return *this;
    }

protected:
    
private:
    static constexpr char start[] = "[";
    static constexpr char end[] = "]";
};

}