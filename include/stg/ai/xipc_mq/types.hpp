/**
 * \file xipc_mq/types.hpp
 *
 * STG XIPC_MQ
 * \details XIPC_MQ type system
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

#include <cstdint>

#pragma once

namespace stg::ai::xipc_mq {

using stg::ai::noncopyable;

using sequence_type = std::size_t;
using capacity_type = std::size_t;

using slot_hdr_sequence_type = std::uint32_t;
using slot_capacity_type = std::uint16_t;

using memsize_type = std::size_t;
using mem_ix_type = uint;

/**
 * Represents XIPC MQ specific error codes
 */
enum class xipc_err: unsigned
{
    OK,         //!< Successful completion
    AGAIN,      //!< Currently no data available
    ARG,        //!< XLIST API call bad arguments
    MEMFAIL     //!< Memory allocation fails
};

/**
 * XIPC MQ producer type
 */
enum class xipc_producer:  uint
{
    SP,
    MP
};

}	// namespace stg::ai::xipc_mq
