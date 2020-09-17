#pragma once

#include <stg/mdp/mdd/common/json_gnr/json_generator/json_types/json_element.hpp>

#include <string>

namespace stg::mdp::mdd::common::json_gnr::types
{

class jnumber : public json_element
{
public:
    jnumber(const std::string& field,
            const std::size_t value,
            const char coma):
           json_element(field, start, end, coma) 
    { json_element::config << (value); }

    virtual ~jnumber() {}

    class builder;

protected:

private:
    static constexpr char start[] = "";
    static constexpr char end[] = "";
};

class jnumber::builder
{
private:
    using self = builder;

public:
    self& field(const std::string& field)
    {
        field_ = field;
        return *this;
    }

    self& value(const std::size_t value)
    {
        value_ = value;
        return *this;
    }

    self& coma(const char coma)
    {
        coma_ = coma;
        return *this;
    }

    jnumber build() const
    { return jnumber(field_, value_, coma_); }

private:
    std::string field_;
    std::size_t value_;
    char coma_;
};

}