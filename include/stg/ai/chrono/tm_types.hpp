/**
 * \file tm_types.hpp
 *
 * STG time types
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

#include <cstdint>
#include <chrono>
#include <ctime>

#include <stg/ai/chrono/tm_cast.hpp>

namespace stg::ai::chrono {

using ns_timestamp_type = std::uint64_t;
using ns_diff_type = std::int64_t;
using tm_cast_type = std::uint64_t;

using stg::ai::tm_cast::sec2ns;

/**
 * Duration cast types
 */
enum class duration_cast
{
    year,
    mon,
    day,
    hr,
    min,
    sec,
    ms,
    us,
    ns
};

struct tm_point;

/**
 * Difference between two time points
 * @details Assumes UTC time, no timezone support would be provided by design
 */
struct tm_diff
{
    using value_type = ns_diff_type;

    /// Default construction
    tm_diff() = default;

    /**
     * Direct timestamps diff initialization
     * @param diff
     */
    explicit tm_diff(value_type const diff):
        diff_{ diff }
    {}

    /**
     * Constructs using timespec value
     * @param diff Time difference represented as timespec
     */
    explicit tm_diff(timespec const& diff):
        tm_diff(diff.tv_sec * sec2ns + diff.tv_nsec)
    {}

    /**
     * Constructs using pair of time points
     * @param Time points pair
     * @note Suggested assumption is that second pair entity is more recent
     */
    tm_diff(std::pair<tm_point const&, tm_point const&> tm_pair);

    /**
     * Duration cast
     * @return Duration value
     */
    template <duration_cast T>
    value_type duration() const;

    /**
     * Assignment for rounded number of specific duration
     * @param Rounded duration value
     * @return Self
     */
    template <duration_cast T = duration_cast::ns>
    tm_diff& assign(value_type const&);

    /**
     * Timespec cast value
     */
    operator timespec() const
    {
        timespec diff;
        diff.tv_sec = diff_ / sec2ns;
        diff.tv_nsec = diff_ % sec2ns;
        return diff;
    }

    /// Implicit access to diff value
    operator value_type() const
    { return diff_; }

    bool operator<(tm_diff const& rhs) const
    { return diff_ < rhs.diff_; }

    /// JSON string representation
    std::string to_json() const;

    /// Provides string representation
    std::string to_string() const;
private:
    /// Time difference storage
    value_type diff_{ 0 };
};

/**
 * Time point
 * @details Uses nanoseconds since epoch paradigm, internally stores UTC nanoseconds
 */
struct tm_point
{
    using value_type = ns_timestamp_type;

    /// Undefined time point value
    constexpr static value_type undefined = std::numeric_limits<value_type>::max();
    constexpr static value_type empty = 0;

    /// Empty time point construction
    tm_point() = default;

    /// Provides default copy constructor
    tm_point(tm_point const&) = default;

    /// Constructs with pre-initialized ns values
    tm_point(value_type const& init):
        tp{ init }
    {}

    /// Access to diff value
    operator value_type() const
    { return tp; }

    /// Access to value
    value_type operator()() const
    { return tp; }

    /// Provides difference between self and given instance (assume self is more recent)
    tm_diff diff(tm_point const& rhs)
    { return tm_diff(tp - rhs.tp); }

    /**
     * Updates time point and returns self reference
     * @return Self
     */
    template <clockid_t T = CLOCK_REALTIME>
    tm_point& now()
    {
        timespec now;
        clock_gettime(T, &now);
        tp = now.tv_sec * sec2ns + now.tv_nsec;
        return *this;
    }

    /// Does resetting time point
    void reset()
    { tp = 0; }

    /**
     * Provides latency measurement from given point in past till known time point value
     * (cached locally)
     * @tparam Time point type
     * @param Ref to time point instance
     */
    template <typename TimePoint>
    value_type get_latency_from(TimePoint const& tp) const
    { return tp - tp.tp; }

    tm_point& operator=(value_type const& value)
    {
        tp = value;
        return *this;
    }

    /**
     * Increase and assign operator
     * @param Delta value
     * @return Self
     */
    tm_point& operator+=(tm_diff const& diff)
    {
        tp += static_cast<tm_diff::value_type>(diff);
        return *this;
    }

    /**
     * Decrease and assign operator
     * @param Delta value
     * @return Self
     */
    tm_point& operator-=(tm_diff const& diff)
    {
        tp -= static_cast<tm_diff::value_type>(diff);
        return *this;
    }

    /// Increase operator
    tm_point operator+(tm_diff const& diff)
    { return tp + static_cast<tm_diff::value_type>(diff); }

    /// Decrease operator
    tm_point operator-(tm_diff const& diff)
    { return tp - static_cast<tm_diff::value_type>(diff); }

    /// Less comparison
    bool operator<(tm_point const& rhs)
    { return tp < rhs.tp; }

    /// Equality comparison
    bool operator==(tm_point const& rhs)
    { return tp == rhs.tp; }

    /// JSON string representation
    std::string to_json() const;

    /// Provides string representation
    std::string to_string() const;
protected:
    /// Time difference storage
    value_type tp{ 0 };
};

tm_diff::tm_diff(std::pair<tm_point const&, tm_point const&> tm_pair)
{
	diff_ = tm_pair.second - tm_pair.first;
}

}  // stg::ai::chrono
