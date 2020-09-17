/**
 * \file slot.hpp
 *
 * STG XIPC MQ
 * \details XIPC MQ slot infrastructure
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

#include <stg/ai/xipc_mq/types.hpp>

namespace stg::ai::xipc_mq {

/**
 * XRING slot
 */
class xslot_t:
        noncopyable
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
    static auto is_committed(unsigned char* payload) const
    { return likely(nullptr != payload) ? payload2origin(payload)->is_committed() : false; }

    /**
     * Checks 'hole_block' flag for slot from user data
     * @return True if 'hole_block' flag set
     */
    static auto is_hole_block(unsigned char* payload) const
    { return likely(nullptr != payload) ? payload2origin(payload)->is_hole_block() : false; }

    /// Construction allowed only with capacity expressed in number of cache lines
    xslot_t(capacity_type chunks, sequence_type seq)
    {
    	hdr.hdr_value = 0;
    	hdr.seq = seq;
    	hdr.n_chunks = chunks;
    }

    /// Acquires type-casted user data block
    template <typename T>
    T* payload()
    { return reinterpret_cast<T*>(&payload_data); }

    /// Sets sub_barrier flag
    void commit()
    { hdr.sub_barrier = true; }

    /**
     * Sets slot as hole block
     */
    void treat_as_hole()
    { hdr.hole_block = true; }

    /**
     * Checks COMMIT status flag
     * @return True if COMMIT flag set, false otherwise
     */
    auto is_committed() const
    { return hdr.sub_barrier; }

    /**
     * Checks that slot represents hole block
     * @return True for hole block
     */
    auto is_hole_block() const
    { return hdr.hole_block; }

protected:
	static constexpr auto payload_offset()
	{ return offsetof(xslot_t, payload_data); }

    /**
     * Transforms userdata address into slot origin address
     * @param payload Pointer to user data
     * @return Pointer to corresponding slot
     */
    static xslot_t* payload2origin(unsigned char* payload)
    { return static_cast<xslot_t*>(payload - payload_offset()); }

    /// Default construction was disabled
    xslot_t() = delete;

    /// Slot header as bitfield
    union
	{
    	struct
		{
    	    const slot_hdr_sequence_type seq : 32;
    	    const slot_capacity_type n_chunks : 16;
    		bool sub_barrier : 1;
    		bool hole_block : 1;
    		uint : 6;
		};

    	/// Flags value, used for resetting
    	std::uint64_t hdr_value { 0 };
	} hdr;

    /// Data payload - fake field, slot must consist at least 64 bytes
	unsigned char payload_data[];

    friend class xipc_mman_t;

    template <typename T>
    friend class xipc_t;

    template <typename T>
    friend class xipc_pub_t;

    template <typename T>
    friend class xipc_rx_t;

    friend class xipc_fetch_t;
};

}  // namespace stg::ai::xipc_mq
