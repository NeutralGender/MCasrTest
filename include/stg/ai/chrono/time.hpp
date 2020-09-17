/**
 * \file time.hpp
 *
 * STG time system
 * \details Time utilities and helpers
 *
 * \author Andrew Shelikhanow (westtrd@txxp.org)
 *
 * NOTICE:  All information contained herein is, and remains the property of mentioned above and
 * its suppliers, if any.
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

#include <ctime>
#include <cstdint>
#include <cstdlib>

#include <stg/ai/chrono/tm_cast.hpp>

namespace stg::ai::chrono {

using stg::ai::tm_cast;

using stg::ai::pow10u;
using stg::ai::num_digits;
using stg::ai::tm_type;
using std::chrono::nanoseconds;

/**
 * Clock IDs
 * @details Clock identifier, primary use case as a time point template parameter
 */
enum clock_id: std::uint8_t
{
    WALL = CLOCK_REALTIME,      //!< Wall realtime clock
    MONO = CLOCK_MONOTONIC,     //!< Monotonic clock
    RAW = CLOCK_MONOTONIC_RAW   //!< Raw monotonic clock
};

/**
 * Timestamp
 * @tparam CLK Clock ID
 */
template <clock_id CLK = clock_id::WALL>
struct time_point
{
    /// Default constructor
    time_point() = default;

    /// Default copy constructor
    time_point(time_point const&) = default;

    /// Construct using struct tm
    time_point(tm point, unsigned ns)
    {
        // TODO: Simplify and make less overhead
        clock_.tv_sec = std::mktime(&point);
        clock_.tv_nsec = ns;
    }

    /// Constructs from timespec
    time_point(timespec const& ts):
        clock_(ts)
    {}

    /// Updates time stamp and returns self
    time_point& now()
    {
        clock_gettime(static_cast<clockid_t>(CLK), &clock_);
        return *this;
    }

    /// Cast to timespec operator
    operator timespec() const
    { return clock_; }

    /// Cast to time_t operator
    operator time_t() const
    { return clock_.tv_sec; }

    /// Casts to std::chrono::nanoseconds
    operator nanoseconds() const
    { return nanoseconds(clock_.tv_sec * tm_cast::sec2ns + clock_.tv_nsec); }

    /// Copy assignment operator
    time_point& operator=(time_point const& rhs)
    {
        clock_ = rhs;
        return *this;
    }

    /// Resets to uninitialized values
    void reset()
    { clock_ = { 0, 0 }; }

    /**
     * Provides latency measurement from given point in past till known time point value
     * (cached locally)
     * @tparam Time point type
     * @param Ref to time point instance
     */
    template <typename TimePoint>
    nanoseconds get_latency_from(TimePoint const&) const;

    /**
     * Provides JSON for time point
     * @return JSON representation
     * @notes Will not implemented and affects linker error for non-wall clock
     * TODO: Provide static assert for non-wall types
     * @details Looks like: "2015-07-14T01:00:00Z"
     */
    std::string to_json(bool include_day = true) const;

private:
    /// All clocks provides nanoseconds resolution
    timespec clock_{ 0, 0 };
};

template<>
std::string time_point<clock_id::WALL>::to_json(bool include_day) const
{
    char buf[64U];
    const char* fmt = "\"%04d%02d%02dT%02d%02d%02d.%09zdZ\"";
    const char* fmt_without_day = "\"%02d%02d%02d.%09zdZ\"";

    tm day = *gmtime_r(&clock_.tv_sec, &day);

    if (include_day)
        snprintf(buf, 64, fmt,
                 day.tm_year + 1900,
                 day.tm_mon + 1,
                 day.tm_mday,
                 day.tm_hour,
                 day.tm_min,
                 day.tm_sec,
                 clock_.tv_nsec);
    else
        snprintf(buf, 64, fmt_without_day,
                 day.tm_hour,
                 day.tm_min,
                 day.tm_sec,
                 clock_.tv_nsec);

   return std::string(buf);
}

/**
 * Time utilities
 */
class chrono
{
public:
    /**
     * Refreshes UTC midnight nanosecond timestamp
     * @return UTC midnight nanoseconds since epoch
     */
    static std::size_t utc_midnight_refresh()
    {

        // Get current wall clock time and emulate UTC midnight
        auto now = std::time(nullptr);
        std::tm utc_tm{0};

        ::gmtime_r(&now, &utc_tm);
        utc_tm.tm_hour = 0;
        utc_tm.tm_min = 0;
        utc_tm.tm_sec = 0;

        midnight_ns_ = ::timegm(&utc_tm) * tm_cast::sec2ns;

        // Calculate timezone offset
        std::tm local_tm{0};
        ::localtime_r(&now, &local_tm);
        localtime_offset_ = local_tm.tm_gmtoff;

        return midnight_ns_;
    }

    /**
     * Provides midnight nanoseconds since epoch value
     * @return Midnight nanoseconds since epoch
     */
    static std::size_t midnight_nsse()
    { return midnight_ns_; }

    /**
     * Converts UTC time to nanoseconds since epoch
     * @param time Expected UTC wall time
     * @return Nanoseconds since epoch
     */
    static std::size_t time2nsse(time_t time)
    { return time * tm_cast::sec2ns; }

    /**
     * Converts wall time to nanoseconds since epoch
     * @param hr Hour
     * @param min Minute
     * @param sec Second
     * @return Nanoseconds since epoch
     */
    static std::size_t wall2nsse(uint hr, uint min, uint sec)
    { return hr * tm_cast::hr2ns + min * tm_cast::min2ns + sec * tm_cast::sec2ns + midnight_ns_; }

    /**
     * Converts wall time to nanoseconds since epoch
     * @param t Expect tm UTC wall clock
     * @return Nanoseconds since epoch
     */
    static std::size_t wall2nsse(std::tm const& t)
    {
        return t.tm_hour * tm_cast::hr2ns +
               t.tm_min * tm_cast::min2ns +
               t.tm_sec * tm_cast::sec2ns +
               midnight_ns_;
    }

    /**
     * Convert wall time and nanoseconds into timespec
     * @param time Wall time
     * @param ns Nanoseconds
     * @return timespec struct
     */
    static timespec wall2timespec(std::tm const& time, uint ns = 0U)
    {
        timespec ts;
        auto nsse = wall2nsse(time);
        ts.tv_sec = nsse / tm_cast::sec2ns;
        ts.tv_nsec = nsse % tm_cast::sec2ns + ns;
        return ts;
    }

    /**
     * Wall time to timespec specialization
     * @param h Hours
     * @param m Minutes
     * @param s Seconds
     * @param ns Nanoseconds
     * @return Timespec value
     */
    static timespec utc2timespec(const uint h, const uint m, const uint s, const uint ns = 0U)
    {
        timespec ts;
        auto nsse = wall2nsse(h, m, s);
        ts.tv_sec = nsse / tm_cast::sec2ns;
        ts.tv_nsec = nsse % tm_cast::sec2ns + ns;
        return ts;
    }

    /**
     * Convert given local wall timestamp th the UTC nanoseconds since midnight
     * @param ts Timestamp value
     * @return UTC nanoseconds since midnight
     */
    static std::size_t timespec2nssm(timespec const& ts)
    { return ((ts.tv_sec - localtime_offset_) * tm_cast::sec2ns + ts.tv_nsec - midnight_ns_); }

    /**
     * Converts timespec to tm_type
     * @param ts Timespec value
     * @return UTC microseconds timestamp value
     */
    static tm_type timespec2tm_type(timespec const& ts)
    { return ts.tv_sec * tm_cast::sec2us + ts.tv_nsec / tm_cast::us2ns; }

    static timespec utc2timespec(std::uint64_t value)
    {
        // Parses SendingTime, pattern is number representation in format yyyymmddhhmmssnnnnn
        // where nnnnn is milliseconds/microseconds/nanoseconds value
        // Note: only up to 5 nanosec digits can safely fit uint64_t in this format
        constexpr static uint date_prefix_len{ 8U };
        constexpr static uint tm_part_len{ 2U };
        constexpr static uint ns_len{ 9U };

        auto timestamp_len = num_digits(value);
        // @warning: assert(num_digits(value) >= 14);

        timestamp_len -= date_prefix_len;
        auto timestamp_value = value % pow10u(timestamp_len);

        timestamp_len -= tm_part_len;
        const uint h = timestamp_value / pow10u(timestamp_len);
        timestamp_value %= pow10u(timestamp_len);

        timestamp_len -= tm_part_len;
        const uint m = timestamp_value / pow10u(timestamp_len);
        timestamp_value %= pow10u(timestamp_len);

        timestamp_len -= tm_part_len;
        const uint s = timestamp_value / pow10u(timestamp_len);
        timestamp_value %= pow10u(timestamp_len);

        return utc2timespec(h, m, s, timestamp_value * pow10u(ns_len - timestamp_len));
    }

    /**
     * Parses SendingTime, pattern is number representation in format hhmmssnnnnnnnnn
     * where nnnnnnnnn is milliseconds/microseconds/nanoseconds value
     * Note: all 9 nanosec digits can safely fit uint64_t in this format
     * @param value Wall UTC time literal value
     * @return Unix timestamp with nanoseconds
     */
    static timespec utctime2timespec(std::uint64_t value)
    {
        constexpr static uint tm_part_len{ 2U };
        constexpr static uint ns_len{ 9U };

        auto timestamp_len = num_digits(value);
        auto timestamp_value = value;

        //@warning: assert(num_digits(value) >= 6);

        timestamp_len -= tm_part_len;
        const uint h = timestamp_value / pow10u(timestamp_len);
        timestamp_value %= pow10u(timestamp_len);

        timestamp_len -= tm_part_len;
        const uint m = timestamp_value / pow10u(timestamp_len);
        timestamp_value %= pow10u(timestamp_len);

        timestamp_len -= tm_part_len;
        const uint s = timestamp_value / pow10u(timestamp_len);
        timestamp_value %= pow10u(timestamp_len);

        return utc2timespec(h, m, s, timestamp_value * pow10u(ns_len - timestamp_len));
    }

    /**
     * Converts UTC timestamp literal into tm_type
     * @param stamp UTC timestamp literal
     * @return UTC expressed in microseconds
     */
    static tm_type utctime2tm_type(std::uint64_t stamp)
    { return timespec2tm_type(utctime2timespec(stamp)); }

private:
    /**
     * Midnight nanoseconds value
     */
    static std::size_t midnight_ns_;

    /**
     * Localtime offset from UTC in  seconds
     * @details Used for timespec adjust to UTC
     */
    static ssize_t localtime_offset_;
};

// Default initializations (NOTE: utc_midnight_refresh() also sets localtime_offset_ value)
ssize_t chrono::localtime_offset_{ 0 };
std::size_t chrono::midnight_ns_{ chrono::utc_midnight_refresh() };

/**
 * Day time wrapper
 */
class daytime_t
{
public:
    /// Constructs daytime as midnight
    daytime_t() :
        midnight_ns_(0)
    {}

    /// Constructs daytime from daytime value
    daytime_t(std::size_t midnight_ns) :
        midnight_ns_(midnight_ns)
    {}

    /// Constructs daytime from timestamp
    daytime_t(timespec ts) :
        midnight_ns_(chrono::timespec2nssm(ts))
    {}

    /// Constructs daytime from another instance
    daytime_t(daytime_t const& instance) :
        midnight_ns_(instance.midnight_ns_)
    {}

    /// Daytime hour
    std::size_t hour() const
    { return midnight_ns_ / tm_cast::hr2ns; }

    /// Daytime minute
    std::size_t minute() const
    { return (midnight_ns_ % tm_cast::hr2ns) / tm_cast::min2ns; }

    /// Daytime second
    std::size_t second() const
    { return (midnight_ns_ % tm_cast::min2ns) / tm_cast::sec2ns; }

    /// Daytime millisecond
    std::size_t ms() const
    { return (midnight_ns_ % tm_cast::sec2ns) / tm_cast::ms2ns; }

    /// Daytime microsecond
    std::size_t us() const
    { return (midnight_ns_ % tm_cast::ms2ns) / tm_cast::us2ns; }

    /**
     * Assignment operator
     * @param rhs Nanoseconds since midnight
     * @return Instance
     */
    daytime_t& operator=(std::size_t rhs)
    {
        midnight_ns_ = rhs;
        return *this;
    }

    /**
     * Returns nanoseconds since midnight stored in daytime_t
     * @return Nanoseconds since midnight
     */
    std::size_t nssm() const
    { return midnight_ns_; }

    /**
     * Updates daytime value with current wall time
     * @details Calls 'clock_gettime with timespec allocated on stack
     * @return New daytime value
     */
    std::size_t update()
    {
        timespec ts;
        clock_gettime(CLOCK_REALTIME, &ts);
        midnight_ns_ = chrono::timespec2nssm(ts);
        return nssm();
    }
private:
    /// Nanoseconds since midnight value
    std::size_t midnight_ns_;
};

}  // namespace stg::ai::chrono
