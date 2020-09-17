/**
 * \file tm_cast.hpp
 *
 * STG time cast
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

namespace stg::ai {

using tm_cast_type = std::uint64_t;

/**
 * Time cast constants
 */
struct tm_cast
{
    /// Microseconds to nanoseconds conversion
    constexpr static tm_cast_type us2ns{ 1000ULL };

    /// Milliseconds to nanoseconds conversion
    constexpr static tm_cast_type ms2ns{ 1000000ULL };

    /// Seconds to nanoseconds conversion
    constexpr static tm_cast_type sec2ns{ 1000000000ULL };

    /// Seconds to microseconds conversion
    constexpr static tm_cast_type sec2us{ 1000000ULL };

    /// Minutes to seconds conversion
    constexpr static tm_cast_type min2sec{ 60U };

    /// Minutes to nanoseconds conversion
    constexpr static tm_cast_type min2ns{ sec2ns * min2sec };

    /// Hours to minutes conversion
    constexpr static tm_cast_type hr2min{ 60U };

    /// Hours to seconds conversion
    constexpr static tm_cast_type hr2sec{ hr2min * min2sec };

    /// Hours to nanosecond conversion
    constexpr static tm_cast_type hr2ns{ sec2ns * hr2sec };

    /// Day to hours conversion
    constexpr static tm_cast_type day2hr{ 24U };

    /// Day to seconds conversion
    constexpr static tm_cast_type day2sec{ day2hr * hr2sec };
};

}  // namespace stg::ai
