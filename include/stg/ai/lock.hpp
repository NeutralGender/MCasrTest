/**
 * \file lock.hpp
 *
 * STG locking primitives
 * \details Simple atomic locker implementation
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

#include <atomic>
#include <mutex>

namespace stg::ai {

/**
 * Spinlock implementation
 */
class spinlock_t
{
public:

	/// Constructor
	spinlock_t():
		v_(ATOMIC_FLAG_INIT)
	{}

    bool try_lock()
    { return not v_.test_and_set(); }

    void lock()
    { while(v_.test_and_set(std::memory_order_acquire)); }

    void unlock()
    { v_.clear(std::memory_order_release); }

private:
    /// Holds locking flag
    alignas(cacheline_sz) std::atomic_flag v_;

    // TODO: Implement padding
};

using scoped_lock = std::scoped_lock<spinlock_t>;
using guard_lock = std::scoped_lock<spinlock_t>;

}  // namespace stg::ai
