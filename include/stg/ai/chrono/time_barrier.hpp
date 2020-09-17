/**
 * \file time_barrier.hpp
 *
 * STG time barrier implementations
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

#include <chrono>

#include <stg/ai/chrono/time.hpp>
#include <stg/ai/chrono/tm_types.hpp>
#include <stg/ai/chrono/tm_cast.hpp>
#include <stg/ai/lock.hpp>

namespace stg::ai::chrono {

using stg::ai::spinlock_t;
using stg::ai::scoped_lock;
using stg::ai::chrono::tm_point;

/**
 * Time - based barrier for implementing minumum guaranteed delay between multi-threaded
 * requests
 */
struct time_barrier
{
    /// Constructs time barrier with given value
    time_barrier(tm_point::value_type shift = 0):
            barrier_sampling{ shift },
            last_barrier{ 0 }
    {}

    /**
     * Checks time barrier
     * @return True if time barrier succeeds
     */
    bool check();

    /// Barrier shift expressed in microseconds
    tm_point::value_type barrier_sampling;

protected:
    /// Last barrier time point
    tm_point last_barrier;

    /// Lock for guard
    mutable spinlock_t guard_lock;
};

inline bool time_barrier::check()
{
    bool ret{ false };
    tm_point tp;
    tp.now();

    if (tp.get_latency_from(last_barrier) >= barrier_sampling)
    {
        // Re-check under guard and register new barrier value
        {
            scoped_lock guard(guard_lock);
            if (tp - last_barrier >= barrier_sampling)
            {
                last_barrier = tp;
                ret = true;
            }
        }
    }
    return ret;
}

/**
 * Fixed range sample exhausting checker
 * @param from More old timestamp
 * @param to More recent timestamp
 * @param sampling Sampling interval in microseconds
 * @return True if timestamps belongs different samples
 */
inline bool tm_sample_exhausted(tm_point const& from,
                                tm_point const& to,
                                tm_point::value_type const& sampling)
{
    const auto from_sample = from /  sampling;
    const auto to_sample = to / sampling;
    return to_sample > from_sample;
}

/**
 * Helper checks exhausting border of wall clock second
 * @param from More old timestamp
 * @param to More recent timestamp
 * @return Condition evaluation result, true for timestamps belongs to different wall second
 * value
 */
bool second_sample_exhausted(tm_point const& from, tm_point const& to)
{ return tm_sample_exhausted(from, to, sec2ns); }

/**
 * Calculates number of seconds between two timestamps bounds
 * @param from First timestamp (beginning)
 * @param to Second timestamp (ends)
 * @return Whole seconds between two timestamps
 * @details Fraction of seconds are ignored:
 * [ 1.99, 2.01 ] -> 1
 * [ 1.00, 1.9999 ] -> 0
 * [ 1.02, 2.22 ] -> 1
 */
std::size_t whole_seconds(tm_point const& from, tm_point const& to)
{
    return to.operator tm_point::value_type() / sec2ns -
           from.operator tm_point::value_type() / sec2ns;
}

}  // stg::ai::chrono
