#pragma once

#include <stg/mdp/mdd/common/json_gnr/json_generator/json_types/json_element.hpp>

#include <string>
#include <iomanip>

namespace stg::mdp::mdd::common::json_gnr::types
{

class jstring : public json_element
{
private:
    using self = jstring;
public:
    jstring(const std::string& field,
            const std::string& value,
            const char coma):
            json_element(field, start, end, coma) 
    { json_element::config << std::quoted(value); }

    virtual ~jstring() {}
    
    void make_string(const std::string& root)
    {}

    class builder;

protected:
    
private:
    static constexpr char start[] = "";
    static constexpr char end[] = "";
};

class jstring::builder
{
private:
    using self = builder;

public:
    self& field(const std::string& field)
    {
        field_ = field;
        return *this;
    }

    self& value(const std::string& value)
    {
        value_ = value;
        return *this;
    }

    self& coma(const char coma)
    {
        coma_ = coma;
        return *this;
    }

    jstring build() const
    { return jstring(field_, value_, coma_); }

private:
    std::string field_;
    std::string value_;
    char coma_;
};

}
