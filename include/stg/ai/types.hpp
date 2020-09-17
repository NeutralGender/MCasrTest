#pragma once

#include <stg/ai/common.hpp>

namespace stg::ai {

/// TSC value
using tsc_type = std::size_t;

/// Generic raw time stamp type for stg::ai::chrono
using tm_type = std::uint64_t;

/// Represents CPU core ID
using core_id_type = std::uint64_t;

/**
 * Helper for unordered map hasher for enums
 * @details Just casts enum fields to its unsigned value
 */
struct enum_hash
{
	template <typename T>
	std::uint64_t operator()(T const& t) const
	{ return static_cast<std::uint64_t>(t); }
};

/**
 * Provides compile-time hash for the zero-terminated alphanumeric string
 * @param x source string
 * @return hash
 * @details Intended for use with enums with predefined string keys like
 * \code{.cpp}
 * enum class someclass: uint { key = str2hash("somekey" };
 * \endcode
 */
constexpr std::size_t str2hash(const char* x)
{ return *x ? *x + (str2hash(x + 1) << 8) : 0; }

/**
 * Provides compile-time hash for the unterminated alphanumeric string
 * @param x source string buffer begin
 * @param len source string buffer or its part len for hash calculating
 * @return hash
 * @details Intended for use for fetching enums from payloads
 */

constexpr std::size_t str2hash_sized(const char* x, uint len)
{ return len ? *x + (str2hash_sized(x + 1, len - 1) << 8) : 0; }

/**
 * Provides compile-time parse for the zero-terminated unsigned numeric string
 * @param x source unsigned numeric string
 * @return Unsigned value
 * @details Intended for use with enums with predefined string keys like
 * \code{.cpp}
 * enum class someclass: uint { key = str2hash("1234" };
 * \endcode
 */
constexpr uint str2uint(const char* x, uint result = 0)
{ return *x ? str2uint(x + 1, (*x - '0') + result * 10) : result; }

/**
 * Enum value to string
 * @tparam EnumName enumeration type name
 * @return Enumeration value
 */
template <typename EnumName>
std::string stringify(EnumName const&);

/**
 * Transforms string into enum
 * @tparam EnumName enumeration type name
 * @return Enumeration value
 */
template <typename EnumName>
EnumName enumerate(std::string const&);

}   // namespace stg::ai
