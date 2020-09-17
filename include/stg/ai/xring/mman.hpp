/**:
 * \file mem.hpp
 *
 * STG XRING
 * \details XRING memory block manager
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

#include <sys/mman.h>

#include <atomic>
#include <cstdint>

#include <stg/ai/lock.hpp>

#include <stg/ai/xring/types.hpp>
#include <stg/ai/xring/conf.hpp>
#include <stg/ai/xring/slot.hpp>

namespace stg::ai::xring {

using stg::ai::noncopyable;
using stg::ai::spinlock_t;

class xring_mman_t:
        noncopyable
{
public:
    /**
     * NULL sequence value
     * @details We need to avoid branch prediction and want to describe 'empty' storage
     * When container is empty, all sequences equals to null sequences.
     * But initial placement is free from branches, because SIZE_MAX + 1 = 0, i.e. index in
     * the chunks virtual array
     */
    constexpr static auto null_seq{ SIZE_MAX };

    /**
     * Constructs XRING memory
     * @param XRING configuration instance
     */
    xring_mman_t(xring_conf const& cfg):
        conf(cfg),
        capacity{ cfg.chunks }
    {

        // Private anonymous mappings with page lock
        int mmap_flags{ MAP_PRIVATE | MAP_ANONYMOUS | MAP_POPULATE };
        if (cfg.use_hugepages)
        {
            // TODO: Verify that hugepages are available
            // Hugepage flag ORable
            mmap_flags |= MAP_HUGETLB;
        }

        // Initialization flow
        memblk = static_cast<u_char*>(mmap(0,
                                           memblk_size(),
                                           PROT_READ | PROT_WRITE,
                                           mmap_flags,
                                           0,
                                           0));
        if ((void*)-1 == memblk)
            // TODO: Handle and log mapping errors
            // Error on mmap() -- treat storage pointer as NULL
            memblk = nullptr;
        else
            clear();
    }

    /**
     * Accesses slot through memory sequence
     * @param acq_mem_seq Acquired memory sequence
     * @return XSLOT pointer, nullptr if no new data or incorrect sequence
     */
    xslot_t* operator[](sequence_type const& acq_mem_seq) const
    {
        return (acq_mem_seq <= msq_hdr) ?
                reinterpret_cast<xslot_t*>(memseq2mem(acq_mem_seq)) :
                nullptr;
    }

    /**
     * Checks whether memory block is empty
     * @return
     */
    bool empty() const
    { return nullptr == memblk; }

    /**
     * Cleans ring, pointers reset to initial position
     */
    void clear()
    {
        //msq = null_seq;
        //msq_hdr = null_seq;
        msq = 0;
        msq_hdr = 0;
        if (nullptr != memblk)
            std::memset(memblk, 0, memblk_size());
    }

    auto const& get_memblk()
    { return ( memblk ); }

    u_char* get_memseq2mem(sequence_type const& mem_seq) const
    { return memblk + memseq2ix(mem_seq) * cacheline_sz; }

    /// Copy of XRING config
    xring_conf const& conf;

    /// Capacity (number of chunks)
    const capacity_type capacity;

protected:
    /**
    * Provides virtual index of cachelines array in the flat memory for given memory sequence
    * @param mem_seq Memory sequence
    * @return Virtual index
    */
    mem_ix_type memseq2ix(sequence_type const& mem_seq) const
    { return mod2(mem_seq, capacity); }

    /**
    * Provides memory address for given memory sequence
    * @param mem_seq Memory sequence
    * @return Address inside storage's memory block
    */
    u_char* memseq2mem(sequence_type const& mem_seq) const
    { return memblk + memseq2ix(mem_seq) * cacheline_sz; }

    /// Helper calculates memory block size
    memsize_type memblk_size() const
    { return conf.chunks * cacheline_sz; }

    /// Lock (used with lock - implemented allocator)
    spinlock_t lck;

    /// Memory sequence (can be with holes, used for indexed access)
    alignas(cacheline_sz) std::atomic<sequence_type> msq{ 0 };

    /// Header sequence, on finished slot allocation MUST be same with msq
    alignas(cacheline_sz) std::atomic<sequence_type> msq_hdr{ 0 };

    /// Points to the begin of page payload storage
    alignas(cacheline_sz) u_char* memblk{ nullptr };

    /// Padded
    alignas(cacheline_sz) char pad[cacheline_sz] = { 0 };
 };

/**
 * XRING memory block descriptor
 */
template <xring_producer ProducerType = xring_producer::SP>
class xring_alloc_t: public xring_mman_t
{
public:
    /**
     * Constructs XRING memory
     * @param XRING configuration instance
     */
    xring_alloc_t(xring_conf const& cfg):
            xring_mman_t{ cfg }
    {}

    /**
     * Slot allocation
     * @param blk_capacity Acquired capacity for user data in bytes
     * @return User data typed pointer, or NULL if allocation fails
     */
    u_char* alloc(memsize_type blk_capacity);
};

template<>
u_char* xring_alloc_t<xring_producer::MP>::alloc(memsize_type blk_capacity)
{
    // Calculate chunks
    const capacity_type chunks_desired = cache_chunks_ceil(xslot_t::hdr_size() + blk_capacity);

    // Update tail memory sequence
    // Please note that msq.fetch_add(...) returns its preceding value i. e. currently processing
    // sequence
    sequence_type active_msq{ 0 };
    sequence_type desired_msq{ 0 };
    bool is_hole_slot{ false };
    do
    {
        // Currently processing
        active_msq = msq.load(std::memory_order_relaxed);

        const capacity_type chunks_avail = capacity - memseq2ix(active_msq);
        const capacity_type chunks_allowed{ std::min(chunks_avail, chunks_desired) };
        desired_msq = active_msq + chunks_allowed;
        is_hole_slot = chunks_allowed < chunks_desired;

        // Try to make changes if 'msq' remains unchanged, otherwise
        [[ likely ]]
        BREAK_IF(msq.compare_exchange_weak(active_msq,
                                           desired_msq,
                                           std::memory_order_relaxed,
                                           std::memory_order_relaxed));
    }
    while (1);

    // Construct slot in-place
    auto placement = memseq2mem(active_msq);

    // Header updation
    // Wait for current header sequence becomes available
    COMPILER_BARRIER;
    while(not msq_hdr.compare_exchange_weak(active_msq,
                                            desired_msq,
                                            std::memory_order_relaxed) );
    COMPILER_BARRIER;

    auto slot = new (placement) xslot_t(chunks_desired, active_msq);

    // Check for sufficient continuous memory
    [[ unlikely ]]
    if (is_hole_slot)
    {
        slot->treat_as_hole();
        slot->commit();
        return alloc(blk_capacity);
    }

    return slot->payload<u_char>();
}

template<>
u_char* xring_alloc_t<xring_producer::MP_LOCK>::alloc(memsize_type blk_capacity)
{
    // Calculate chunks
    const capacity_type chunks_desired = cache_chunks_ceil(xslot_t::hdr_size() + blk_capacity);

    // Update tail memory sequence
    // Please note that msq.fetch_add(...) returns its preceding value i. e. currently processing
    // sequence
    sequence_type active_msq{ 0 };
    bool is_hole_slot{ false };
    xslot_t* slot{ nullptr };

    COMPILER_BARRIER;
    {
        scoped_lock lock( lck );

        // Currently processing
        active_msq = msq;

        const capacity_type chunks_avail = capacity - memseq2ix(active_msq);
        const capacity_type chunks_allowed{ std::min(chunks_avail, chunks_desired) };
        const sequence_type desired_msq{ active_msq + chunks_allowed };

        is_hole_slot = chunks_allowed < chunks_desired;
        msq = desired_msq;

        // Construct slot in-place
        auto placement = memseq2mem(active_msq);
        slot = new (placement) xslot_t(chunks_desired, active_msq);

        // Header updation
        // Header data commit - wait for previous header data written, 'msq_hdr' MUST same with
        // currently allocating memory sequence
        while( not msq_hdr.compare_exchange_weak(active_msq,
                                                 desired_msq,
                                                 std::memory_order_relaxed) );
    }
    COMPILER_BARRIER;

    // Check for sufficient continuous memory
    [[ unlikely ]]
    if (is_hole_slot)
    {
        slot->treat_as_hole();
        slot->commit();
        return alloc(blk_capacity);
    }

    return slot->payload<u_char>();
}

template<>
u_char* xring_alloc_t<xring_producer::SP>::alloc(memsize_type blk_capacity)
{
    const size_t chunks_desired = cache_chunks_ceil(xslot_t::hdr_size() + blk_capacity);

    // Construct slot in-place
    const sequence_type active_msq{ msq };
    const capacity_type chunks_avail = capacity - memseq2ix(active_msq);
    const capacity_type chunks_allowed{ std::min(chunks_avail, chunks_desired) };
    const bool is_hole_slot{ chunks_allowed < chunks_desired };

    auto placement = memseq2mem(active_msq);
    auto slot = new (placement) xslot_t(chunks_allowed, active_msq);

    // Header data commit
    msq += chunks_allowed;
    msq_hdr.store(msq, std::memory_order_relaxed);

    [[ unlikely ]]
    if (is_hole_slot)
    {
        slot->treat_as_hole();
        slot->commit();
        return alloc(blk_capacity);
    }

    return slot->payload<u_char>();
}

}  // namespace stg::ai::xring
