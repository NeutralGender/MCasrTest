/**
 * \file xring.hpp
 *
 * STG XRING
 * \details XRING API
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
#include <string>
#include <unordered_set>

#include <stg/ai/log.hpp>

#include <stg/ai/xring/types.hpp>
#include <stg/ai/xring/conf.hpp>
#include <stg/ai/xring/slot.hpp>
#include <stg/ai/xring/fetch.hpp>
#include <stg/ai/xring/mman.hpp>

#define LOG_TAG "[stg.ai.xring] "

/*
template <typename T>
class xring_fetch_t;
*/

namespace stg::ai::xring {

using stg::ai::noncopyable;

template <xring_producer ProducerType = xring_producer::SP>
class xring_t:
        noncopyable
{
public:

    /**
     * Constructs XRING
     * @param XRING configuration instance
     * @param tag XRING identification tag
     */
    xring_t(xring_conf const& cfg, std::string const& tag = "XR"):
        tag( tag ),
        conf( cfg ),
        allocator{ cfg }
    {}

    /**
     * Destructor
     */
    ~xring_t()
    {
        // Delete all fetchers
        for (auto it: fetchers)
        {
            // TODO: it.second->signal_disable();
            delete it;
        }
        fetchers.clear();

        // Destroy memory
        LOG_INF(LOG_TAG "destructor: @%p, '%s' destroyed", this, tag.c_str());
    }

    /**
     * XRING fetcher creator
     * @param tag Tag for fetcher
     * @return Fetcher instance
     * @throws If we're attempting to create cursor on non-empty ring
     */
    xring_fetch_t& alloc_fetcher(std::string const& tag = "XF")
    {
        // TODO: RAISE_IF(not mem->empty(), "Cursors can be given only for empty ring");
        xring_fetch_t* fetcher = new xring_fetch_t(allocator, tag);
        xring_fetch_t* result = *(fetchers.emplace(fetcher).first);
        return *result;
    }

    /**
     * Allocates memory block in the XRING
     * @param capacity Acquired capacity in bytes
     * @return 0 if acquire fails, otherwise typed pointer to user data in a slot memory
     * @note Actually allocates minimum possible number of cache lines
     */
    template <typename T>
    T* alloc(memsize_type capacity)
    { return reinterpret_cast<T*>(allocator.alloc(capacity)); }

    /**
     * Commits a slot
     * @param payload Payload buffer address
     * @return Completion result
     */
    template <typename T>
    void commit(T* at)
    { xslot_t::commit(at); }

    /**
     * Cleanups ring
     * @warning Protection against running threads implemented, but strongly
     * recommended stop publisher and consumer when calling this member
     */
    void clear()
    { allocator.clear(); }

    /**
     * Subscript operator for memory cell sequence-based
     * @param seq Memory cell sequence
     * @return Tuple of pointer (nullptr on error), capacity of data and completion result
     */
    auto operator[](sequence_type const& mem_seq) const
    {
        const xslot_t* slot = (allocator)[mem_seq];
        xring_err err{ xring_err::AGAIN };

        [[ likely ]]
        if (nullptr != slot)
        {
            [[ likely ]]
            if (mem_seq == slot->seq() and slot->is_committed())
                err = xring_err::OK;
            else
                slot = nullptr;

        }

        return std::make_tuple(slot, err);
    }


    /**
     * XRING tag
     * @return Tag string reference
     */
    const std::string tag;

    /// XRING configuration
    const xring_conf conf;

    /// Ring memory allocator
    xring_alloc_t<ProducerType> allocator;


protected:
    /// Storage for cursors pointers
    std::unordered_set<xring_fetch_t*> fetchers{};

private:
    /// Default construction was disabled
    xring_t() = delete;
};

}  // namespace stg::ai::xring

#undef LOG_TAG
