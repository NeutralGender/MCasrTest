/**
 * \file fetch.hpp
 *
 * STG XRING fetch interface
 * \details XRING fetch interface
 *
 * \author Andrew Shelikhanow (westtrd@txxp.org)
 * \copyright STG
 *
 * NOTICE:  All information contained herein is, and remains the property of
 * mentioned above and its suppliers, if any.
 *
 * The intellectual and technical concepts contained herein are proprietary to
 * mentioned above and its suppliers and may be covered by U.S. and Foreign
 * Patents, patents in process, and are protected by trade secret or copyright
 * law.
 *
 * Dissemination of this information or reproduction of this material is
 * strictly forbidden unless prior written permission is obtained from
 * proprietors.
 *
 */

#pragma once

#include <stg/ai/ai.hpp>
#include <stg/ai/xring/mman.hpp>

#include <tuple>

#include <stg/ai/xring/types.hpp>
#include <stg/ai/xring/slot.hpp>
#include <stg/ai/xring/xring.hpp>

namespace stg::ai::xring {

using stg::ai::noncopyable;

/// XRING fetcher API
class xring_fetch_t: 
        noncopyable
{
public:
    /**
     * Destructor
     */
    ~xring_fetch_t() = default;

    /**
     * Acquires a next available slot from the XRING
     * @param capacity Acquired capacity in bytes
     * @return 0 if acquire fails, otherwise typed pointer to user data in a slot memory
     * @note Actually allocates minimum possible number of cache lines
     */
    template <typename T>
    std::tuple<T*, xring_err> pop()
    {
        // Fetch on last memory sequence if available
        xslot_t* slot = mman[last_memsq];
        //std::cout << "pop slot" << slot << std::endl;
        xring_err err{ xring_err::AGAIN };
        [[ likely ]]
        if (nullptr != slot and last_memsq == slot->seq() and slot->is_committed())
        {
            err = xring_err::OK;

            // Done, go new sequence
            last_memsq += slot->hdr.properties.n_chunks;

            // Handle skipping of the hole block
            [[ unlikely ]]
            if (slot->is_hole_block())
                return pop<T>();
            else
                return std::make_tuple(slot->payload<T>(), err);
        }
        return std::make_tuple(nullptr, err);
    }

protected:
    /**
     * Constructs XRING cursor
     * @param XRING instance
     * @details Can be called only from XRING instance
     */
    xring_fetch_t(xring_mman_t& mman, const std::string& tag = "XC"):
            mman{ mman },
            tag( tag )
    {}

    /// Ring instance
    xring_mman_t& mman;

    /// Tag for cursor
    const std::string tag;

    /// Last head memory cell sequence
    alignas (cacheline_sz) sequence_type last_memsq{ 0 };
    alignas (cacheline_sz) sequence_type pad{ 0 };
private:
    /// Default construction was disabled
    xring_fetch_t() = delete;

    template <xring_producer T>
    friend class xring_t;
};

}  // namespace stg::ai::xring
