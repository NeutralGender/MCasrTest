/**
 * \file xring/conf.hpp
 *
 * STG XRING
 * \details XRING configuration
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

#include <cstdint>

#include <stg/ai/log.hpp>
#include <stg/ai/xring/types.hpp>

#define LOG_TAG "[axl.xring.cfg] "

namespace stg::ai::xring {

/**
 * XRING user-preferred configuration
 * \details Used for XRING initialization
 */
class xring_conf
{
public:

	/// 1024 bytes multiplier
	constexpr static memsize_type mul1k{ 1024 };

	/// Default block is 2MB
	constexpr static memsize_type default_capacity{ 2 * mul1k * mul1k };

    /// Constructs default configurator
    xring_conf() = default;

    /**
     * Verifies correctness of configuration
     * @return Validity
     */
    bool verify() const
    {
        bool rc = 0 != chunks;

        if (not rc)
            LOG_ERR(LOG_TAG "verify(): Configuration verify fails", "");

        return rc;
    }

    /**
     * Helper that allows sets up chunks nearest
     * @param capacity_mib Desired capacity in megabytes
     */
    void setup_capacity(memsize_type capacity_mib)
    {
        const uint acq_chunks = capacity_mib * mul1k * mul1k / cacheline_sz;
        chunks = next_pow2(acq_chunks);
    }

    void setup_chunk( const std::size_t chunk_count )
    {
        chunks = next_pow2( chunk_count );
    }

    /// Number of chunks (cachelines) in storage
    /// \details Default value will represent 2MB Linux hugepages
    std::size_t chunks{ default_capacity / cacheline_sz };

    /// Use mmap() hugepages
    bool use_hugepages{ false };
};

}  // namespace stg::ai::xring

#undef LOG_TAG
