/**:
 * \file mman.hpp
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

#include <stg/ai/xipc_mq/types.hpp>
#include <stg/ai/xipc_mq/conf.hpp>
#include <stg/ai/xipc_mq/slot.hpp>

namespace stg::ai::xipc_mq {

class xipc_mman_core_t
{
public:
    /**
     * Accesses slot through memory sequence
     * @param acq_mem_seq Acquired memory sequence
     * @return XSLOT pointer, nullptr if no new data or incorrect sequence
     */
    xslot_t* operator[](sequence_type const& acq_mem_seq) const
    { return (acq_mem_seq <= msq_hdr) ? static_cast<xslot_t*>(memseq2mem(acq_mem_seq)) : nullptr; }

    /**
     * Checks whether memory block is empty
     * @return
     */
    bool empty() const
    { return nullptr == memblk; }

    /// Copy of XRING config
    xipc_conf const& conf;

    /// Capacity (number of chunks)
    const capacity_type capacity;

    template <typename T>
    friend class xipc_t;

protected:
    /**
     * NULL sequence value
     * @details We need to avoid branch prediction and want to describe 'empty' storage.
     * When container is empty, all sequences equals to null sequences.
     * But initial placement is free from branches, because SIZE_MAX + 1 = 0, i.e. index in
     * the chunks virtual array
     */
    constexpr static auto null_seq{ SIZE_MAX };

    /**
     * Constructs XIPC MQ memory
     * @param XIPC MQ configuration instance
     */
    xipc_mman_core_t(xipc_conf const& cfg):
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
        memblk = mmap(0, memblk_size(), PROT_READ | PROT_WRITE, mmap_flags, 0, 0);
        if ((void*)-1 == memblk)
            // TODO: Handle and log mapping errors
            // Error on mmap() -- treat storage pointer as NULL
            memblk = nullptr;
        else
            std::memset(memblk, 0, memblk_size());
    }

    /**
     * Destroys page and deallocates its memory
     */
    ~xipc_mman_core_t()
    {
        // Deallocate storage if allocated depend on allocation type
        [[ likely ]]
        if (nullptr != memblk)
        {
            munmap(memblk, memblk_size());
            memblk = nullptr;
        }
    }

    /**
     * Cleans ring, pointers reset to initial position
     */
    void clear()
    {
        msq = null_seq;
        msq_hdr = null_seq;
        std::memset(memblk, 0, memblk_size());
    }

    /// Helper calculates memory block size
    memsize_type memblk_size() const
    { return conf.chunks * cacheline_sz; }

    /// Memory sequence (can be with holes, used for indexed access)
    alignas(cacheline_sz)
    std::atomic<sequence_type> msq{ null_seq };

    /// Header commit sequence
    alignas(cacheline_sz)
    std::atomic<sequence_type> msq_hdr{ null_seq };

    /// Points to the begin of page payload storage
    alignas(cacheline_sz)
    u_char* memblk{ nullptr };

    /// Padded
    alignas(cacheline_sz)
    char pad[cacheline_sz] = { 0 };
private:
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
    auto memseq2mem(sequence_type const& mem_seq) const
    { return memblk + memseq2ix(mem_seq) * cacheline_sz; }
};

/**
 * XIPC MQ memory block descriptor
 */
template <xipc_producer P>
class xipc_mman_t:
        noncopyable,
        public xipc_mman_core_t
{
public:
    /**
     * Slot allocation
     * @param blk_capacity Acquired capacity for user data in bytes
     * @return User data typed pointer, or NULL if allocation fails
     */
    template <typename T>
    T* alloc(memsize_type const& blk_capacity);

protected:

    /**
     * Constructs XIPC MQ memory
     * @param XIPC MQ configuration instance
     */
    xipc_mman_t(xipc_conf const& cfg):
        xipc_mman_core_t{ cfg }
    {}

    /**
     * Destroys page and deallocates its memory
     */
    ~xipc_mman_t() = default;

    /// Helper calculates memory block size
    memsize_type memblk_size() const
    { return conf.chunks * cacheline_sz; }
};

template<>
template <typename T>
T* xipc_mman_t<xipc_producer::MP>::alloc(memsize_type const& blk_capacity)
{
    // Calculate chunks
    const auto chunks = cache_chunks_ceil(xslot_t::payload_offset() + blk_capacity);

    // Update tail memory sequence
    // TODO: Decide for low-level migration
    const capacity_type msq_entry = msq.fetch_add(chunks, std::memory_order_relaxed);

    // Construct slot in-place
    const auto placement{ xipc_mman_core_t::memseq2mem(msq_entry)  };
    auto slot = new (placement) xslot_t(chunks, msq_entry);

    // Header data commit
    // TODO: Decide for low-level migration
    [[ unlikely ]]
    if (not atomic_compare_exchange_weak(&msq_hdr, &msq_entry, msq_entry))
        [[ unlikely ]]
        while (not atomic_compare_exchange_weak(&msq_hdr, &msq_entry, msq_entry));

    // Check for sufficient continuous memory
    // Note that 'capacity' is 1-based index, while 'n_mem_ix' is 0-based, so it will work only
    // for 'greatest than' condition
    const auto n_mem_ix{ memseq2ix(msq_entry) };
    const bool has_cont_blk = (capacity - n_mem_ix) > chunks;
    [[ unlikely ]]
    if (not has_cont_blk)
    {
        slot->treat_as_hole();
        return xipc_mman_t<xipc_producer::MP>::alloc<T>(blk_capacity);
    }
    return slot->payload<T>();
}

template<>
template <typename T>
T* xipc_mman_t<xipc_producer::SP>::alloc(memsize_type const& blk_capacity)
{
    // Calculate chunks
    const auto chunks = cache_chunks_ceil(xslot_t::payload_offset() + blk_capacity);

    // Construct slot in-place
    const sequence_type active_msq = msq;
    const auto placement{ xipc_mman_core_t::memseq2mem(active_msq)  };
    auto slot = new (placement) xslot_t(chunks, active_msq);

    // Header data commit
    msq += chunks;
    msq_hdr = msq;

    // Check for sufficient continuous memory
    // Note that 'capacity' is 1-based index, while 'n_mem_ix' is 0-based, so it will work only
    // for 'greatest than' condition
    const auto n_mem_ix{ memseq2ix(active_msq) };
    const bool has_cont_blk = (capacity - n_mem_ix) > chunks;
    [[ unlikely ]]
    if (not has_cont_blk)
    {
        slot->treat_as_hole();
        return xipc_mman_t<xipc_producer::SP>::alloc<T>(blk_capacity);
    }
    return slot->payload<T>();
}

}  // namespace stg::ai::xipc_mq
