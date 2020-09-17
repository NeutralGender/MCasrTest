/**
 * \file date.hpp
 *
 * Date utilites
 *
 * \author Andrew Shelikhanow (westtrd@txxp.org)
 * \copyright STG
 *
 * NOTICE:  All information contained herein is, and remains the property of mentioned above
 * and its suppliers, if any.
 *
 * The intellectual and technical concepts contained herein are proprietary to mentioned above and
 * its suppliers and may be covered by U.S. and Foreign Patents, patents in process, and are
 * protected by trade secret or copyright law.
 *
 * Dissemination of this information or reproduction of this material is strictly forbidden unless
 * prior written permission is obtained from proprietors.
 */

#pragma once

#include <stg/ai/ai.hpp>
#include <stg/ai/types.hpp>
#include <ctime>
#include <string>

#include <json/json.hpp>

#include <stg/ai/chrono/tm_cast.hpp>

namespace stg::ai::chrono {

using stg::ai::tm_cast::day2sec;
using stg::ai::tm_cast::sec2us;

/// Weekday literals
enum class weekday: uint
{
    SUNDAY = 7,
    MONDAY = 1,
    TUESDAY,
    WEDNESDAY,
    THURSDAY,
    FRIDAY,
    SATURDAY,
    CNT = SUNDAY
};

/// Weekday wrapper
class weekday_t
{
public:
    weekday_t(weekday wd) :
            wd_{ wd }
    {}

    /**
     * Provides weekday string representation
     */
    operator std::string() const;

    /**
     * Parses ISO weekday from string into instance
     * @param Weekday ISO string
     */
    void parse(std::string const&);

    /**
     * Weekday assignment
     * @param Weekday to assign
     * @return Self instance reference
     */
    weekday_t& operator=(weekday wd)
    {
      wd_ = wd;
      return *this;
    }

    /**
     * Copy assignment operator
     * @param instance Another weekday instance
     * @return Self instancce reference
     */
    weekday_t& operator=(weekday_t const& rhs)
    {
        wd_ = rhs.wd_;
        return *this;
    }
private:

    /// Weekday enumerator value
    weekday wd_;
};

/**
 * Date type
 */
struct date_type
{
    using date_value_type = time_t;
    using date_literal = uint;

    date_type() = default;

    /**
     * Constructs from numeric literal looks like yyyymmdd (for example, 20170312)
     * @param rhs Numeric date literal without time
     */
    date_type(date_value_type const& rhs):
            value_{ rhs }
    {}

    date_type(date_literal const& rhs)
    {
        tm utc = { 0 };
        utc.tm_year = rhs / 10000 - 1900;
        utc.tm_mon = (rhs % 10000) / 100 - 1;
        utc.tm_mday = rhs % 1000000;

        value_ = timegm(&utc);
    }

    /// Parse constructor
    date_type(const char* it)
    { parse(it); }

    /// Equality operator
    bool operator==(date_type const& rhs) const
    { return value_ == rhs.value_; }

    /// Inequality operator
    bool operator!=(date_type const& rhs) const
    { return not operator==(rhs); }

    /// Less operator
    bool operator<(date_type const& rhs) const
    { return value() < rhs.value(); }

    /// Greater operator
    bool operator>(date_type const& rhs) const
    { return value() > rhs.value(); }

    /// Less or equal operator
    bool operator<=(date_type const& rhs) const
    { return value() <= rhs.value(); }

    /// Used as hash value to support use as key in unordered maps
    std::size_t operator()(date_type const& h) const
    { return h.value_; }

    /// Calculates full days between instance and another day (can be negative)
    int diff(date_type const& second) const
    { return std::difftime(second.value(), value()) / day2sec; }

    /// Provides string representation as yyyy-mm-dd
    std::string to_string() const
    {
        tm out;
        char buf[16];

        gmtime_r(&value_, &out);
        strftime(buf, sizeof(buf), "%Y-%m-%d", &out);
        return buf;
    }

    /// Conversion to tm_type
    tm_type utc_midnight_usec() const
    { return value_ * sec2us; }

    /// Date numeric value
    date_value_type value() const
    { return value_; }

    /// Parses day from string pattern yyyy-mm-dd into UTC midnight seconds value
    void parse(char const* it)
    {
        tm tp = { 0 };
        value_ = 0;
        if (3 == std::sscanf(it, "%d-%d-%d", &tp.tm_year, &tp.tm_mon, &tp.tm_mday))
        {
            tp.tm_year -= 1900;
            tp.tm_mon -= 1;
            value_ = timegm(&tp);
        }
    }

    /// Adjust date to specified number of days forward or backward (negative)
    void adjust(int days = 1)
    { value_ += days * day2sec; }

    /// Provides numeric literal looks like yyyymmdd (for example, 20170312)
    date_literal literal() const
    {
        tm utc;
        gmtime_r(&value_, &utc);

        return (utc.tm_year + 1900) * 10000 + (utc.tm_mon + 1) * 100 + utc.tm_mday;
    }
private:

    /// Date value type (UTC seconds at midnight of representing date)
    date_value_type value_{ 0 };
};

std::ostream& operator<< (std::ostream& stream, date_type const& date)
{
    stream << date.to_string();
    return stream;
}

struct date_range_type
{
    date_range_type() = default;

    date_range_type(json const& j):
        begin_{ j[0].get<std::string>().c_str() },
        end_{ j[1].get<std::string>().c_str() }
    {}

    date_range_type(date_type const& begin, date_type const& end):
            begin_( begin ),
            end_( end_ )
    {}

    date_range_type(date_range_type const& rhs):
            begin_( rhs.begin_ ),
            end_( rhs.end_ )
    {}

    /**
     * Date range parser specialization
     * @param rhs Date range JSON array
     * @return Self
     */
    date_range_type& operator=(json const& rhs)
    {
        begin_.parse(rhs[0].get<std::string>().c_str());
        end_.parse(rhs[1].get<std::string>().c_str());
        return *this;
    }

    date_type const& begin() const
    { return begin_; }

    date_type const& end() const
    { return end_; }

private:
    date_type begin_;
    date_type end_;
};

std::ostream& operator<< (std::ostream& stream, date_range_type const& range)
{
    stream << range.begin() << ", "  << range.end() << std::endl;
    return stream;
}

}  // namespace stg::ai::chrono
