#pragma once

#include <stg/mdp/mdd/common/json_gnr/json_generator/json_types/separator.hpp>

#include <string>
#include <vector>

#include <iomanip>
#include <sstream>

namespace stg::mdp::mdd::common::json_gnr::types
{

class json_element
{
public:
    json_element( const std::string& name,
                  const std::string& start,
                  const std::string& end,
                  const char coma ) : 
                  sep(start, end),
                  coma(coma)
    {
        config << std::quoted(name);
        config << delim;
        config << sep.start();
    }
    
    json_element(const std::string& start,
                 const std::string& end,
                 const char coma) :
                 sep(start, end), 
                 coma(coma)
    { config << sep.start(); }

    virtual ~json_element()
    { config << sep.end() << coma; }

    static std::stringstream config;

protected:
    separator sep;
private:
    const char coma;
    static constexpr char delim[] = ":";
};

std::stringstream json_element::config;

}