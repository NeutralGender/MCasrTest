/**
 * \file range.hpp
 *
 * STG custom ranges and its iterators support
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

#include <cstddef>

namespace stg::ai {

/**
 * Unsigned range implementation
 * @details It is a helper for range loop
 */
class urange
{
public:

    /// Implements range iterator
    class iterator
    {
    public:
        /// Dereference operator
        uint operator *() const
        { return i_; }

        /// Pre-increment operator
        const iterator& operator++()
        {
            ++i_;
            return *this;
        }

        /// Post - increment iterator
        iterator operator++(int)
        {
            iterator copy(*this);
            ++i_;
            return copy;
        }

        /// Equality operator
        bool operator ==(const iterator& other) const
        { return i_ == other.i_; }

        /// Inequality operator
        bool operator !=(const iterator &other) const
        { return i_ != other.i_; }

    protected:
        /// Constructor
        iterator(uint start) : i_(start) {}

    private:

        /// Unsigned iterator value
        uint i_;

        friend class urange;
    };

   /// Acquires begin iterator
   iterator begin() const
   { return begin_; }

   /// Acquires end iterator
   iterator end() const
   { return end_; }

   /// Construct unsigned range
   urange(uint  begin, uint end):
           begin_(begin),
           end_(end)
   {}

   /// Constructs 0-based unsigned range
   urange(uint end):
           urange(0, end)
   {}
private:

   /// Begin iterator
   iterator begin_;

   /// End iterator (constant)
   const iterator end_;
};

}  // namespace stg::ai
