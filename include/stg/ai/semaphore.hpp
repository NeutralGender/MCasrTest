/**
 * \file semaphore.hpp
 *
 * STG decreasing semaphore class
 *
 * \authors Gennady Shibaev
 * 			Andrew Shelikhanow
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

#include <atomic>
#include <condition_variable>
#include <mutex>

namespace stg::ai {

/**
 * Decreasing semaphore
 * @details Usually handles when we need barrier before numbers of threads are started
 * @note Constructed wait handle for single event by default
 */
class ai_semaphore_t
{
public:

	ai_semaphore_t() = default;

	/**
	 * Construct semaphore
	 * @param counter Decreasing counter
	 */
    ai_semaphore_t(unsigned counter):
    		semaphore_value{ counter }
    {}

    /**
     * Woke semaphore
     * @details Usually notifies semaphore about an event
     */
    void wake()
    {
        std::unique_lock<std::mutex> guard( mx );
        switch (semaphore_value)
        {
            [[ unlikely ]]
            case 0:
                break;
            [[ likely ]]
            default:
                --semaphore_value;
                break;
        }
        cv.notify_one();
    }

    /**
     * Waits for semaphore opens (control variable becomes zero)
     */
    void wait()
    {
        std::unique_lock<std::mutex> guard( mx );
        while (0 < semaphore_value)
        	cv.wait(guard);
    }

protected:
    /// Semaphore wait handle
    std::condition_variable cv;

    /// Semaphore value (decreasing)
    std::atomic<unsigned> semaphore_value{ 1 };

    /// Mutex for CV and guards
    std::mutex mx;
};

}
