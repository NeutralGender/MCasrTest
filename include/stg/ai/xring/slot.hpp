/**
 * \file slot.hpp
 *
 * STG XRING
 * \details XRING slot infrastructure
 *
 * \author Andrew Shelikhanow (westtrd@txxp.org)
 * \copyright STG
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

#include <cstdint>

#include <stg/ai/xring/types.hpp>

namespace stg::ai::xring {

using stg::ai::noncopyable;

/**
 * XRING slot
 */
class xslot_t: noncopyable
{
public:
	/// Acquires slot pointer from user data pointer
	template <typename T>
	static xslot_t* slot(T* payload)
	{ return payload2origin(reinterpret_cast<u_char*>(payload)); }

    /**
     * Sets 'sub_barrier' flag for slot from user data
     * @param payload Pointer to user data (excluded slot header)
     */
    static auto commit(unsigned char* payload)
    { payload2origin(payload)->commit(); }

    /**
     * Checks 'sub_barrier' flag for slot from user data
     * @return True if 'sub_barrier' flag set
     */
    static auto is_committed(unsigned char* payload)
    { return likely(nullptr != payload) ? payload2origin(payload)->is_committed() : false; }

    /**
     * Checks 'hole_block' flag for slot from user data
     * @return True if 'hole_block' flag set
     */
    static auto is_hole_block(unsigned char* payload)
    { return likely(nullptr != payload) ? payload2origin(payload)->is_hole_block() : false; }

    /// Construction allowed only with capacity expressed in number of cache lines
    xslot_t(capacity_type chunks, sequence_type seq)
    {
    	hdr.hdr_value = 0;
    	hdr.properties.seq = seq;
    	hdr.properties.n_chunks = chunks;
    }

    /// Provides sequence of slot
    constexpr sequence_type seq() const
    { return hdr.properties.seq; }

    /// Acquires type-casted user data block
    template <typename T>
    T* payload()
    { return reinterpret_cast<T*>(&payload_data); }

    /// Sets sub_barrier flag
    void commit()
    { hdr.properties.sub_barrier = true; }

    /**
     * Sets slot as hole block
     */
    void treat_as_hole()
    { hdr.properties.hole_block = true; }

    /**
     * Checks COMMIT status flag
     * @return True if COMMIT flag set, false otherwise
     */
    bool is_committed() const
    { return hdr.properties.sub_barrier; }

    /**
     * Checks that slot represents hole block
     * @return True for hole block
     */
    bool is_hole_block() const
    { return hdr.properties.hole_block; }

    /**
     * Getter returns enclosed struct 
     * @return struct __flags_type
     */
    auto const& flags_struct()
    { return hdr.properties; }

protected:
	static constexpr uint hdr_size()
	{ return offsetof(xslot_t, payload_data); }

    /**
     * Transforms userdata address into slot origin address
     * @param payload Pointer to user data
     * @return Pointer to corresponding slot
     */
    static xslot_t* payload2origin(unsigned char* payload)
    { return reinterpret_cast<xslot_t*>(payload - hdr_size()); }

    /// Default construction was disabled
    xslot_t() = delete;

    /// Slot header as bitfield
    union __header_type
	{
        __header_type() = default;

    	struct __flags_type
		{
    	    slot_hdr_sequence_type seq : 32;
    	    slot_capacity_type n_chunks : 16;
    		bool sub_barrier : 1;
    		bool hole_block : 1;
    		uint : 6;
		} properties;

    	/// Flags value, used for resetting
    	std::uint64_t hdr_value { 0 };
	} hdr;

    /// Data payload - fake field, slot must consist at least 64 bytes
    #pragma GCC diagnostic push
    #pragma GCC diagnostic ignored "-Wpedantic"
    unsigned char payload_data[];
    #pragma GCC diagnostic pop

    friend class xring_mman_t;

    template <xring_producer ProducerType>
    friend class xring_t;

    template <xring_producer ProducerType>
    friend class xring_alloc_t;

    friend class xring_fetch_t;
};

}  // namespace stg::ai::xring
