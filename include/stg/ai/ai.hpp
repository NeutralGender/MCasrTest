/**
 * \file ai.hpp
 *
 * STG core header entry point
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

#include <csignal>
#include <mutex>
#include <iostream>

#ifndef STG_BUILD_CONF_DROP_JSON
#include <json/json.hpp>
#endif

#include <stg/ai/common.hpp>
#include <stg/ai/manage.hpp>
#include <stg/ai/types.hpp>

using json = nlohmann::json;

namespace stg::ai {

/**
 * AXL Ai Engine singleton
 */
class ai_core_t
{
public:
    /// Provides TX Ai Engine instance with optional initialization
    static ai_core_t& instance()
    {
        [[ unlikely ]]
        if (nullptr == core_)
            core_ = new ai_core_t;

        return *core_;
    }

    /**
     * Activates SIGINT handler
     * @return self
     */
    ai_core_t& sigint_activate()
    {
        std::lock_guard<std::mutex> lock(m_);
        if (not sigint_active_)
        {
            signal(SIGINT, sigint_handler);
            sigint_active_ = true;
        }
        return *this;
    }

    /**
     * Deactivates SIGINT handler
     * @return
     */
    ai_core_t& sigint_deactivate()
    {
        std::lock_guard<std::mutex> lock(m_);
        std::signal(SIGINT, SIG_DFL);
        return *this;
    }

    /// Termination instances register
    ai_term_t term_register;

private:
    /**
     * SIGINT handler
     * @param signal Signal number (currently ignored)
     */
    static void sigint_handler(int signal)
    {
        // Call entire termination list
        instance().term_register.terminate_all();
        instance().sigint_deactivate();
    }


    /// Singleton instance
    static ai_core_t* core_;

    /// Used for guard locks
    std::mutex m_;

    /// True for activated SIGINT handler
    bool sigint_active_{ false };
};

ai_core_t* ai_core_t::core_{nullptr};

}  // namespace stg::ai
