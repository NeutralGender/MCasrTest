/**
 * \file fetch.hpp
 *
 * STG XIPC MQ fetch interface
 * \details XIPC MQ fetch interface
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

#include <tuple>

#include <stg/ai/xipc_mq/types.hpp>
#include <stg/ai/xipc_mq/mman.hpp>
#include <stg/ai/xipc_mq/slot.hpp>

namespace stg::ai::xipc_mq {

/// XIPC MQ fetcher
class xipc_fetch_t:
        noncopyable
{
public:
    /**
     * Destructor
     */
    ~xipc_fetch_t() = default;

    /**
     * Acquires a next available slot from the XIPC
     * @param capacity Acquired capacity in bytes
     * @return { nullptr, xipc_err::FAIL } if acquire fails, otherwise pointer to user data
     * in a slot memory
     * @note Actually allocates minimum possible number of cache lines
     */
    template <typename T>
    std::tuple<const T const*, xipc_err> pop() const
    {
        // Fetch on last memory sequence if available
        xslot_t* slot{ nullptr };
        xipc_err err{ xipc_err::OK };
        std::tie(slot, err) = mem[last_memsq];

        switch (err)
        {
            [[ likely ]]
            case xipc_err::OK:
            {
                // Done, go new sequence
                last_memsq += slot->hdr.n_chunks;

                // Handle skipping of the hole block
                [[ unlikely ]]
                if (slot->is_hole_block())
                    return pop<T>();
                else
                    return { slot->payload<const T const>(), err };
                break;
            }
            [[ unlikely ]]
            default:
                break;
        }
        return { nullptr, err };
    }

protected:
    /**
     * Constructs XIPC MQ cursor/fetcher
     * @param XIPC MQ instance
     * @details Can be called only from XRING instance
     */
    xipc_fetch_t(xipc_mman_core_t const& mem, constexpr std::string& tag = "XC"):
            mem{ mem },
            tag( tag )
    {}

    /// Ring instance
    xipc_mman_core_t const& mem;

    /// Tag for cursor
    constexpr std::string tag;

    /// Free memory cell sequence
    alignas (cacheline_sz)
    sequence_type last_memsq{ 0 };

    /// Pad
    alignas (cacheline_sz)
    constexpr sequence_type pad{ 0 };
private:
    /// Default construction was disabled
    xipc_fetch_t() = delete;
};

}  // namespace stg::ai::xipc_mq
