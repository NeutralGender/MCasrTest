/**
 * \file common.hpp
 *
 * STG common stuff
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

#include <string>
#include <cstdio>
#include <iostream>
#include <arpa/inet.h>

/// Conditional return clause
#define RET_IF(x) if (x) return

/// Conditional continue clause
#define CONT_IF(x) if (x) continue
#define CONT_IF_NOT(x) if (not x) continue


/// Helper for flow break clause
#define BREAK_IF(x) if (x) break

template <typename T>
typename std::enable_if<std::is_integral<T>::value, std::string>::type
to_hexstring(T const& value)
{
	char buf[24] { 0 };
	std::sprintf(buf,"0x%X", value);
	return buf;
}

namespace stg::ai::net {

constexpr uint STG_IP_ADDR_BUFLEN{ 16 };

/**
 * Produces internet address from string
 * @param s Input string formatted as IPv4 address
 * @return Numeric internet address
 */
inline in_addr_t string2inet(std::string const& s)
{
    sockaddr_in sa;
    inet_pton(AF_INET, s.c_str(), &(sa.sin_addr));

    return sa.sin_addr.s_addr;
}

/**
 * Produces string value from internet address
 * @param addr IP address value (in the network byte order)
 * @return String representing IP address like "AAA.BBB.CCC.DDD"
 */
inline std::string inaddr2string(in_addr_t const& addr)
{
    char buf[STG_IP_ADDR_BUFLEN];
    inet_ntop(AF_INET, &addr, buf, STG_IP_ADDR_BUFLEN);
    return buf;
}

}  // namespace stg::ai::net

/**
 * Calculates modulo for divisor in the set of powers of 2
 * @param number Number to get modulus
 * @param divisor Divisor (must be power of 2)
 * @return Modulo
 */
template <typename N, typename D>
auto mod2(N const& number, D const& divisor)
{ return (number & (divisor - 1)); }

/**
 * Provides next power of 2 value for given number
 * @param n Number for which next power of 2 was requested
 * @return Next power of 2 value
 */
constexpr auto next_pow2(unsigned int n)
{
    unsigned count = 0;

    // First n in the below condition is for the case where n is 0
    if (n && !(n & (n - 1)))
        return n;

    while( n != 0)
    {
        n >>= 1;
        count += 1;
    }

    return 1u << count;
}

/// Compiler barrier - prevents compiler instructions reordering
#define COMPILER_BARRIER             asm volatile("" ::: "memory")

/// Memory barrier - prevents CPU and compiler instructions reordering
#define MEMORY_BARRIER               asm volatile("mfence" ::: "memory")

/// SMP memory barrier - prevents CPU and compiler instructions reordering
#define SMP_MEMORY_BARRIER           __asm__ __volatile__("": : :"memory")

/**
 * Branch prediction helpers
 */
#ifdef __GNUC__
#define likely(x)       __builtin_expect((x),1)
#define unlikely(x)     __builtin_expect((x),0)
#else
#define likely(x)       (x)
#define unlikely(x)     (x)
#endif

#ifndef unused
# define unused        __attribute__ ((__unused__))
#endif

/**
 * Force inline
 */
#ifdef __STG_INLINES__FORCE__
#define forceinline inline __attribute__((__always_inline__))
#else
#define forceinline inline
#endif

// TODO: this must be a macro
// otherwise we cannot use it in the case:
// RAISE_IF(arr.size() != 0 && arr.front() == 0, "zero arr[0]")
//
#ifndef __STG_AI_RAISE_DISABLE__
template <typename ...Args>
void RAISE_IF(bool cond, const char* fmt, Args&& ...args)
{
    if (cond)
    {
        std::fprintf(stderr, fmt, args...);
        std::cerr << std::endl << std::flush;
        throw std::runtime_error("RAISE_IF");
    }
}

void RAISE_IF(bool cond, const char* arg)
{
        if (cond)
        {
            std::fprintf(stderr, "%s", arg);
            std::cerr << std::endl << std::flush;
            throw std::runtime_error("RAISE_IF");
        }
}

#endif
#define RAISE(s) std::runtime_error("raise: " s)

/**
 * Defines CPU cache line size
 * TODO: Get value from system-wide macros
 */
constexpr static uint cacheline_sz{ 64U };

/**
 * Rounded upper number of cache line chunks
 * @param bytes Required bytes
 * @return Number of cache lines required for given bytes
 */
constexpr uint cache_chunks_ceil(uint bytes)
{ return ((bytes / cacheline_sz) + ((bytes % cacheline_sz) > 0)); }

constexpr uint chunk_ceil(uint bytes, uint boundary)
{ return bytes + (bytes % boundary); }

template <typename Iterable, typename Func>
void each(Iterable& iterable, Func func)
{
    for (auto& it: iterable)
        func(it);
}

/**
 * Iterates iterable with passing index to given functor
 * @param iterable Subject to iterate through
 * @param func Functor that takes contained data and an index
 */
template <class Iterable, typename Func>
void each_indexed(Iterable& iterable, Func func)
{
    uint ix{ 0 };
    for (auto& it: iterable)
    {
        func(it, ix);
        ++ix;
    }
}

/**
 * Value output into stream implementation for empty arguments
 * @param out Output stream
 */
template <typename Stream>
void stream_out_impl(Stream& out)
{ out << std::endl; }

/**
 * Value output into stream implementation
 * @param out Output stream
 * @param v First value in the arguments list
 * @param args Remain arguments
 */
template <typename Stream, typename T, typename ...Args>
void stream_out_impl(Stream& out, const T &v, const Args& ... args)
{
    out << v << " ";
    stream_out_impl(out, args...);
}

/**
 * Values output into stream entry point (space-separated)
 * @param out Output stream
 * @param value First value in the arguments list
 * @param args Remain arguments
 */
template <typename Stream, typename T, typename ...Args>
void stream_output(Stream& out, T const& value, Args const& ...args)
{ stream_out_impl(out, args...); }

namespace stg::ai {

	/**
	 * Parent for non-copyable instances
	 * @details Usage: class someclass : noncopyable ...
	 */
	class noncopyable
	{

		protected:
			noncopyable() = default;

		private:
	    	/// Assignment operator was disabled
	    	noncopyable& operator=(noncopyable const&) = delete;

	    	/// Copy constructor disabled
	    	noncopyable(noncopyable const&) = delete;
	};

	struct nonmovable:
	        noncopyable
	{
	    nonmovable(nonmovable&&) = delete;
	    nonmovable & operator=(noncopyable&&) = delete;
	};

	/**
	 * Wrapper for disabling non-placement construction for single instance and for arrays
	 * @details Usage:
	 * class someclass:
	 *      public require_emplace ...
	 */
	class require_emplace
	{
	public:
	    /**
	     * Placement new implementation
	     * @param Acquired size (unused)
	     * @param p Placement address
	     */
	    void* operator new(std::size_t, void * p)
	    { return p; }

	protected:

	    /// Uses default constructor
	    constexpr require_emplace() = default;

	    /**
	     * Prevents standard 'new' operator
	     * @param Number of bytes to allocate
	     */
	    void* operator new(std::size_t);

	    /**
	     * Prevents standard 'new' operator for arrays
	     * @param Number of bytes to allocate
	     */
	    void* operator new[](std::size_t);
	};

	constexpr std::size_t pow10u(uint exponent)
	{ return (exponent == 0) ? 1 : (10 * pow10u(exponent-1)); }

	/// Power of 10 for signed exponent
	constexpr double pow10i(int exponent)
	{
	    switch (exponent)
	    {
	    case 0: return 1;
	    default: return (exponent > 0) ? 10 * pow10i(exponent - 1) : double(.1) * pow10i(exponent + 1);
	    }
	}

	/// Power of 2 for unsigned exponent
	constexpr std::size_t pow2(uint exponent)
	{ return (exponent == 0) ? 1 : (2 * pow2(exponent-1)); }

	template <typename T, typename M>
	T round_dn(T val, M mod)
	{ return (std::fabs(mod) == 0.0) ? val : T(mod * std::floor(val / mod)); }

	template <typename T, typename M>
	T round_up(T val, M mod)
	{  return (fabs(mod) == 0.0) ? val : T(mod * std::ceil(val / mod)); }

	template <typename T>
	T round_dn(T val)
	{ return round_dn(val, 1); }

	template <typename T>
	T round_up(T val)
	{ return round_up(val, 1); }

	/**
	 * Determines number of decimal digits in an unsigned value
	 * @param value Unsigned value
	 * @todo Move to tx.ai
	 */
	uint num_digits(size_t value)
	{

	    using digits = uint;
	    using digits_chg_value = std::uint64_t;

	#define DIGITS(bit) static_cast<digits>(std::log10(exp2(bit)) + 1)
	#define NEXT_DGTS(bit) static_cast<digits_chg_value>(exp10(DIGITS(bit)))
	#define BIT(bit) { DIGITS(63 - bit), NEXT_DGTS(63 - bit) }

	    #pragma GCC diagnostic push
	    #pragma GCC diagnostic ignored "-Wnarrowing"
	    const static std::pair<digits, digits_chg_value> tbl[]
	    {
	        BIT(0), BIT(1), BIT(2), BIT(3), BIT(4), BIT(5), BIT(6), BIT(7),
	        BIT(8), BIT(9), BIT(10), BIT(11), BIT(12), BIT(13), BIT(14), BIT(15),
	        BIT(16), BIT(17), BIT(18), BIT(19), BIT(20), BIT(21), BIT(22), BIT(23),
	        BIT(24), BIT(25), BIT(26), BIT(27), BIT(28), BIT(29), BIT(30), BIT(31),
	        BIT(32), BIT(33), BIT(34), BIT(35), BIT(36), BIT(37), BIT(38), BIT(39),
	        BIT(40), BIT(41), BIT(42), BIT(43), BIT(44), BIT(45), BIT(46), BIT(47),
	        BIT(48), BIT(49), BIT(50), BIT(51), BIT(52), BIT(53), BIT(54), BIT(55),
	        BIT(56), BIT(57), BIT(58), BIT(59), BIT(60), BIT(61), BIT(62), BIT(63)
	    };
	    #pragma GCC diagnostic pop

	#undef BIT
	#undef NEXT_DGTS
	#undef DIGITS

	    size_t idx = __builtin_clzl(value);
	    return tbl[idx].first + (value >= tbl[idx].second);
	}

}  // namespace stg::ai
