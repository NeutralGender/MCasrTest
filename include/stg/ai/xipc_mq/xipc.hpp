/**
 * \file xipc.hpp
 *
 * STG XIPC MQ
 * \details XIPC MQ
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

#include <stg/ai/xipc_mq/types.hpp>
#include <stg/ai/xipc_mq/conf.hpp>
#include <stg/ai/xipc_mq/slot.hpp>
#include <stg/ai/xipc_mq/fetch.hpp>
#include <stg/ai/xipc_mq/mman.hpp>

#define LOG_TAG "[axl.ai.xipc] "

namespace stg::ai::xipc_mq {

// TODO: Separate:
// - Publisher logic
// - Memory allocator logic
// - Client logic
//
// And try to implement common header for management purposes

template <xipc_producer P>
class xipc_t:
		noncopyable
{
public:

    /**
     * Constructs XRING
     * @param XRING configuration instance
     * @param tag XRING identification tag
     */
    xipc_t(xipc_conf const& cfg, std::string const& tag = "XR"):
        conf( cfg ),
        tag( tag ),
        mem{ conf }
    {}

    /**
     * Destructor
     */
    ~xipc_t()
    {
        // Delete all fetchers
        for (auto it: fetchers)
        {
            // TODO: it.second->signal_disable();
            auto instance = it;
            delete instance;
        }
        fetchers.clear();

        // Destroy memory
        LOG_INF(LOG_TAG "destructor: @%p, '%s' destroyed", this, tag.c_str());
    }

    /**
     * XIPC fetchers fabric
     * @param tag Tag for fetcher
     * @return Fetcher instance
     * @throws If we're attempting to create cursor on non-empty ring
     */
    xipc_fetch_t const& alloc_fetcher(std::string const& tag = "XF")
    {
        RAISE_IF(not this->mem.empty(), "Cursors can be given only for empty ring");
        auto fetcher = new xipc_fetch_t(this->mem, tag);
        auto result = fetchers.emplace(fetcher);
        return *result.second;
    }

    /**
     * Allocates memory block in the XRING
     * @param capacity Acquired capacity in bytes
     * @return 0 if acquire fails, otherwise typed pointer to user data in a slot memory
     * @note Actually allocates minimum possible number of cache lines
     */
    template <typename T>
    T* alloc(memsize_type capacity)
    { return this->mem.alloc<P>(capacity); }

    /**
     * Commits a slot
     * @param payload Payload buffer address
     * @return Completion result
     */
    template <typename T>
    void commit(T* at)
    { xslot_t::commit(at); }

    /**
     * XIPC MQ tag
     * @return Tag string reference
     */
    constexpr std::string tag;

    /**
     * Cleanups ring
     * @warning Protection against running threads implemented, but strongly recommended stop
     * publisher and consumer when calling
     */
    void clear()
    {
        [[ likely ]]
        if (not this->mem.memory)
            this->mem.clear();
    }

    /// XIPC MQ configuration
    const xipc_conf conf;

    /**
     * Subscript operator for memory cell sequence-based
     * @param seq Memory cell sequence
     * @return Tuple of pointer (nullptr on error), capacity of data and completion result
     */
    auto operator[](sequence_type const& mem_seq) const
    {
        const xslot_t* slot = (*mem)[mem_seq];
        xipc_err err{ xipc_err::AGAIN };

        [[ likely ]]
        if (nullptr != slot)
        {
            [[ likely ]]
            if (mem_seq == slot->hdr.seq and slot->hdr.sub_barrier)
                err = xipc_err::OK;
            else
                slot = nullptr;

        }

        return std::make_tuple(slot, err);
    }

    /// IPC memory block
    xipc_mman_t<P> mem;

protected:
    /// Storage for cursors pointers
    std::unordered_set<xipc_fetch_t*> fetchers{};

private:
    /// Default construction was disabled
    xipc_t() = delete;
};

}  // namespace stg::ai::xipc_mq

#undef LOG_TAG
