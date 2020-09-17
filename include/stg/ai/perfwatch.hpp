/**
 * \file perfwatch.hpp
 *
 * STG performance measurement helpers
 *
 * \author Andrew Shelikhanow (westtrd@txxp.org)
 * \copyright STG
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
 *
 */

#pragma once

#include <stg/ai/ai.hpp>
#include <stg/ai/chrono/tm_cast.hpp>

namespace stg::ai::chrono {

    /**
     * TSC utilities container
     */
    struct tsc_util
    {
        /**
         * Transforms timespec values into nanoseconds
         * @param ts timespec value
         * @return Nanoseconds value
         */
        static inline tsc_type timespec2nsec(timespec const& ts)
        { return ts.tv_sec * stg::ai::tm_cast::sec2ns + ts.tv_nsec; }

        /// Fetches raw TSC for now
        static inline tsc_type fetch_raw_tsc()
        {
            timespec ts{ 0 };
            clock_gettime(CLOCK_MONOTONIC_RAW, &ts);
            return timespec2nsec(ts);
        }
    };


}

/**
 * Obtains and returns current TSC value
 */
static inline unsigned long long rdtsc(void)
{
    unsigned long long hi, lo;
    __asm__ __volatile__ ("rdtsc" : "=a"(lo), "=d"(hi));
    return ( (unsigned long long)lo)|( ((unsigned long long)hi)<<32 );
}


/**
 * Places current TSC value into address specified by 'destination'
 * @param destination Destination address t o place TSC value
 */
static inline void rdtsc_(unsigned long& destination)
{ __asm__ volatile (".byte 0x0f, 0x31" : "=A" (destination)); }
