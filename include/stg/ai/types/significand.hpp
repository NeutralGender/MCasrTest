/**
 * \file significand.hpp
 *
 * STG type system
 * \details Significand types
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
 *
 */

#pragma once

#include <stg/ai/ai.hpp>
#include <stg/ai/common.hpp>

#include <cstdint>
#include <cmath>
#include <utility>

namespace stg::ai {

/// Power of 10 for signed exponent
constexpr double ipow10(int exponent)
{
    switch (exponent)
    {
    case 0: return 1;
    default: return (exponent > 0) ? 10 * ipow10(exponent - 1) : double(.1) * ipow10(exponent + 1);
    }
}

/// Unsigned significand
class significand_t
{
public:
    /// Mantissa value type
    typedef std::int64_t mantissa_type;

    /// Exponent type
    typedef std::int8_t exponent_type;

    /// Significand value type
    typedef std::pair<mantissa_type, exponent_type> value_type;
    

    /// Default construction
    significand_t() = default;
    
    /**
     * Copy construction
     * @param other Another instance
     */
    significand_t(significand_t const& other):
            significand_(other.significand_)
    {}
    
    /**
     * Move construction semantic using significand value
     * @param value Significand value
     */
     significand_t(value_type&& value):
            significand_(value)
    {}
    
    /**
     * Move construction semantic using other significand instance
     * @param other Other significand instance
     */
    significand_t(significand_t&& other):
            significand_(std::move(other.significand_))
    {}
    
    /**
     * Constructs instance for saving backward compatibility
     * @param value Integer part
     * @param precision Precision as power of 10
     * @details For example, 1234.567 must represented as value = 1234567
     * and precision = -3
     */
    significand_t(std::int64_t value, std::int8_t precision) :
        significand_( std::make_pair(value, precision) )
    {}

    /// Equality operator
    bool operator==(significand_t const& rhs) const
    { return significand_ == rhs.significand_; }

    /// Inequality operator
    bool operator!=(significand_t const& rhs) const
    { return not operator==(rhs); }

    /// Gives significand's mantissa
    mantissa_type mantissa() const
    { return significand_.first; }

    /// Gives significand's exponent
    exponent_type exponent() const
    { return significand_.second; }

    /// Casts instance into significand's value
    operator value_type&&()
    { return std::move(significand_); }

    /// Casts instance into float value
    operator float() const
    { return mantissa() * ipow10(exponent()); }

    /// Casts instance into double value
    operator double() const
    { return mantissa() * ipow10(exponent()); }
    
    significand_t& operator=(significand_t const& rhs)
    { 
        significand_.first = rhs.significand_.first;
        significand_.second = rhs.significand_.second;
        defined_ = rhs.defined_;
        return *this;
    }

    /// True if zero significand value
    bool zero() const
    { return 0 == mantissa(); }

    /// True if non-zero significand value
    bool nonzero() const
    { return 0 != mantissa(); }

    /// Formats significand as JSON value
    std::string to_json() const
    { return "[" + std::to_string(mantissa()) + "," + std::to_string(exponent()) + "]"; }
    
    /// To integer conversion operator
    operator ssize_t() const
    { return  mantissa(); }
    
    /// To unsigned integer conversion operator
    explicit operator std::size_t() const
    { return  mantissa(); }

    /// Acquires string representation
    explicit operator std::string() const
    { return std::to_string(static_cast<double>(*this)); }

    /// Defined/undefined property
    bool defined() const
    { return defined_; }

private:
    /// Significand value
    value_type significand_{ 0, 0 };

    /// Is value defined?
    bool defined_{false};

};

/// Asset price type
using price_type = significand_t;

/// Rate type (can be negative
using rate_type = significand_t;

/// Yield (can be negative
using yield_type = significand_t;

}  // namespace stg::ai
