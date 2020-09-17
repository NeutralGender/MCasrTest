#pragma once

#include <string>

namespace stg::mdp::mdd::common::json_gnr::types
{

class separator
{
public:
    separator(const std::string& start,
              const std::string& end):
              start_(start), end_(end) {}

    separator() {}

    virtual ~separator() {}

    const std::string& start()
    { return (start_); }

    const std::string& end()
    { return (end_); }

private:
    const std::string start_;
    const std::string end_;
};

}

