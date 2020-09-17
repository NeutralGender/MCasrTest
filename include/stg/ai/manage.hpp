/**
 * \file manage.hpp
 *
 * STG Process wait/wake/termination infrastructure
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

#include <condition_variable>
#include <atomic>
#include <list>
#include <mutex>

namespace stg::ai {

/**
 * AXL Ai base interface
 * @details Presents basic automated interface
 */
class ai_t
{
public:

    /// Destructor
    virtual ~ai_t()
    {}

    /// Instance termination
    virtual void terminate()  = 0;
};

/**
 * Wait handle
 */
class ai_wait_t:
        public ai_t
{
public:

    /// Destructor
    ~ai_wait_t() = default;

    /// Wakeup
    void wake()
    {
        wake_signaller = true;
        cv.notify_one();
    }

    /// Blocking wait
    void wait()
    {
        std::unique_lock<std::mutex> lock(mx);
        wake_signaller = false;
        while (not wake_signaller)
            cv.wait(lock);
    }

    /// Termination
    virtual void terminate()
    { wake(); }
protected:
    /// Spurious wakeup handle
    volatile std::atomic<bool> wake_signaller{ false };

    /// Condition variable
    std::condition_variable cv;

    /// Locking mutex
    std::mutex mx;
};

/**
 * Termination list
 */
class ai_term_t final
{
public:
    /// Default constructor
    ai_term_t() = default;

    /// Adds instance into the termination list
    void add(ai_t& instance)
    { ai_list.emplace_back(&instance); }

    void terminate(ai_t& instance)
    {
        instance.terminate();
        remove(&instance);
    }

    /// Terminates all stored elements
    void terminate_all()
    { for (auto& it: ai_list) terminate(*it); }
private:
    /// Removes instance pointer from the termination list
    void remove(ai_t const* instance)
    {
        auto it = std::find(ai_list.begin(), ai_list.end(), instance);
        if (ai_list.end() != it)
            ai_list.erase(it);

        // TODO: Not found, log warning message
    }

    /// Termination instances storage
    std::list<ai_t*> ai_list{};
};

}  // namespace stg::ai
