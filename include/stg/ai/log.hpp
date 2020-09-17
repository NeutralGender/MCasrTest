/**
 * \file log.hpp
 *
 * \details Logger infrastructure
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
 */

#pragma once

#include <stg/ai/ai.hpp>

#include <sys/syscall.h>
#include <unistd.h>
#include <cstdarg>
#include <ctime>
#include <string>
#include <cstring>
#include <tuple>
#include <cstdio>
#include <thread>
#include <utility>

#include <stg/ai/filesystem.hpp>

using stg::ai::path_t;

#define CON_RED(s) "\033[31m" s "\033[0m"
#define CON_GREEN(s) "\033[32m" s "\033[0m"
#define CON_YELLOW(s) "\033[33m" s "\033[0m"

/// Error events logging - always logged
#define STG_LOG_LEVEL_ERROR           0

/// Warning events logging -- can be suppressed but doesn't recommended
#define STG_LOG_LEVEL_WARN            1

/// Informational events logging level
#define STG_LOG_LEVEL_INF             2

/// Audit events logging level
#define STG_LOG_LEVEL_AUDIT           3

/// Trace events logging level
#define STG_LOG_LEVEL_TRACE           4

/// Debug events logging level
#define STG_LOG_LEVEL_DEBUG           5

/**
 * If build system isn't defines logging level, it will be TX_LOG_LEVEL_DEBUG
 * by default
 */
#ifndef STG_LOG_LEVEL
#define STG_LOG_LEVEL STG_LOG_LEVEL_AUDIT
#endif

#define LOG_FLUSH log_t::instance()->flush()

/// System log - always logged
#define LOG(fmt, ...) log_t::instance()->logx(log_t::instance()->sys_,  \
                                                 "",                           \
                                                 "%s" fmt "\n",                \
                                                 __VA_ARGS__)

#define LOGQ(s) log_t::instance()->logx(log_t::instance()->sys_, "", "%s%s\n", s)

/// PANIC log - impossible to disable logging
#define LOG_PANIC(fmt, ...) log_t::instance()->logx(log_t::instance()->syslog_,   \
                                                     "PANIC",                         \
													 "%s" fmt "\n",                   \
													 __VA_ARGS__)

#define LOGQ_PANIC(s) log_t::instance()->logx(log_t::instance()->sys_, "PANIC", "%s%s\n", s)

/// Error log level
#define LOG_ERR(fmt, ...) log_t::instance()->logx(log_t::instance()->sys_,        \
												   "ERR",                             \
												   "%s" fmt "\n",                     \
												   __VA_ARGS__)

#define LOGQ_ERR(s) log_t::instance()->logx(log_t::instance()->sys_, "ERR", "%s%s\n", s)

#if STG_LOG_LEVEL >= STG_LOG_LEVEL_WARN
/// Warning log level
#define LOG_WARN(fmt, ...) log_t::instance()->logx(log_t::instance()->sys_,       \
                                                      "WARN",                           \
                                                      "%s" fmt "\n",                    \
                                                      __VA_ARGS__)
#define LOGQ_WARN(s) log_t::instance()->logx(log_t::instance()->sys_, "WARN", "%s%s\n", s)

#else
#define LOG_WARN(...)
#define LOGQ_WARN(...)
#endif

#if STG_LOG_LEVEL >= STG_LOG_LEVEL_INF
/// Info log level
#define LOG_INF(fmt, ...) log_t::instance()->logx(log_t::instance()->sys_,        \
                                                     "INFO",                            \
                                                     "%s" fmt "\n",                     \
                                                     __VA_ARGS__)
#define LOGQ_INF(s) log_t::instance()->logx(log_t::instance()->sys_, "INF", "%s%s\n", s)
#else
#define LOG_INF(...)
#define LOGQ_INF(...)
#endif

/// Audit log level
#if STG_LOG_LEVEL >= STG_LOG_LEVEL_AUDIT
#define LOG_AUDIT(fmt, ...) log_t::instance()->logx(log_t::instance()->audit_,    \
                                                       "AUDIT",                         \
                                                        "%s" fmt "\n",                  \
                                                        __VA_ARGS__)
#define LOGQ_AUDIT(s) log_t::instance()->logx(log_t::instance()->audit_, "AUDIT", "%s%s\n", s)
#else
#define LOG_AUDIT(...)
#define LOGQ_AUDIT(...)
#endif

/// Trace log level
#if STG_LOG_LEVEL >= STG_LOG_LEVEL_TRACE
#define LOG_TRACE(fmt, ...) log_t::instance()->logx(log_t::instance()->audit_,    \
                                                       "TRACE",                         \
                                                       "%s" fmt "\n",                   \
                                                       __VA_ARGS__)
#define LOGQ_TRACE(s) log_t::instance()->logx(log_t::instance()->audit_, "TRACE", "%s%s\n", s)
#else
#define LOG_TRACE(...)
#define LOGQ_TRACE(...)
#endif

/** Debug log level */
#if STG_LOG_LEVEL >= STG_LOG_LEVEL_DEBUG
#define LOG_DBG_FLUSH log_t::instance()->flush()
#define LOG_ASSERT(expr, fmt, ...)      \
    if (expr)                           \
    {                                   \
        LOG_DBG(fmt, __VA_ARGS__);      \
        assert(expr);                   \
    }
#define LOG_DBG(fmt, ...) log_t::instance()->logx(log_t::instance()->debug_,  \
                                                "DBG",                                  \
                                                "%s" fmt "\n",                          \
                                                __VA_ARGS__)
#define LOG_DBG(s) log_t::instance()->logx(log_t::instance()->debug_, "DBG", "%s%s\n", s)
#else
#define LOG_DBG(...)
#define LOG_DBG_FLUSH
#define LOG_ASSERT(expr, fmt, ...)
#endif


/** Conditional warning logging */
#if STG_LOG_LEVEL >= STG_LOG_LEVEL_WARN
#define LOG_WARN_IF(expr, fmt, ...)                 \
    {                                               \
        if (expr)                                   \
            LOG_WARN(fmt, __VA_ARGS__);             \
    }
#else
#define LOG_WARN_IF(...)
#endif

/** Log message into stderr, default: disabled */
#ifndef STG_LOG_CON_DISABLED
#define LOG_CON(fmt, ...) log_t::instance()->to_stderr("%s" fmt "\n", __VA_ARGS__)
#define LOGQ_CON(s) log_t::instance()->to_stderr("%s%s\n", s)
#else
#define LOG_CON(fmt, ...)
#define LOGQ_CON(fmt, ...)
#endif

#define LOG_CON_FORCE(fmt, ...) log_t::instance()->to_stderr("%s" fmt "\n", __VA_ARGS__)
#define LOGQ_CON_FORCE(s) log_t::instance()->to_stderr("%s%s\n", s)

#define LOG_TAG "[axl.ai.log] "

/**
 * Instantiates log with default suffix
 */
#define LOG_INSTANTIATE(path, key)                                  \
        log_t::instance()->init(std::make_tuple(path,                \
        										std::string(key),    \
												log_t::suffix(),  \
												false));

#define LOG_INSTANTIATE_SFX(path, key, sfx, append) \
        log_t::instance()->init(std::make_tuple(path, key, sfx, append));

class log_t
{
public:
    /// Nanoseconds to microseconds transform constant
    constexpr static size_t ns2us = 1000U;
    constexpr static size_t ns2ms = 1000000U;

    /// String for throwing error
    static constexpr const char*
	init_error = LOG_TAG "FATAL: cannot initialize logging subsystem";

    /// Acquires logger instance
    static log_t* instance()
    {
        if (unlikely(not is_instance()))
            instance_ = new log_t;
        return instance_;
    }

    /// Returns true if logger active
    static bool is_instance()
    { return nullptr != instance_; }

    /**
     * Produces hex dump as line-breaked inside string
     * @param buf Points to the raw data to dump
     * @param len Length of raw data block
     * @return Hex dump string representation
     */
    static std::string hexdump_ex(std::uint8_t const* buf, uint len)
    {
        // Guard empty datagram
        if (0 == len)
            return "";

        std::string out("");

    #define QWORD2LIST(x) x[0], x[1], x[2], x[3], x[4], x[5], x[6], x[7], \
                           x[8], x[9], x[10], x[11], x[12], x[13], x[14], x[15]
    #define __HEXDUMP_NONPRINT__ '.'

        constexpr static const char* fmt = "> %04X:  "
                                           "%02X %02X  %02X %02X  %02X %02X  %02X %02X"
                                           " | %02X %02X  %02X %02X  %02X %02X  %02X %02X"
                                           " [%s\n";
        uint offset = 0;

        // Iterate over data buffer
        while (offset < len)
        {
            char printout[128] = { 0 };
            std::uint8_t data[17] = { 0 };

            // ASCII part representation
            std::uint8_t ascii[17] = { 0 };

            // Remaining part of buffer, used for tail processing
            auto remain = (len - offset >= 16) ? 16 : (len - offset);

            // Copy data
            memcpy(ascii, buf + offset, remain);
            memcpy(data, buf + offset, remain);

            for (uint idx = 0; idx < 16; ++idx)
            {
                auto ch = ascii[idx];
                if (ch <= 0x1F or ch == 0x7F)
                    ascii[idx] = __HEXDUMP_NONPRINT__;
            }

            // Dump to buffer
            snprintf(reinterpret_cast<char*>(printout),
                     sizeof(printout),
                     fmt,
                     offset,
                     QWORD2LIST(data),
                     ascii);
            out += std::string(printout);

            // Increase offset for non-ceiling portions handling
            offset += remain;
        }

        return out;

    #undef QWORD2LIST
    #undef __HEXDUMP_NONPRINT__
    }

    static std::string hexdump_string(const uint8_t* data, u_int32_t size)
    {
        constexpr std::size_t PRINT_BYTES_PER_LINE { 16 };
        constexpr char fmt[] = "%02x";

        char hexpair[3] = {0};
        std::string out;

        int offset = 0;
        int nlines = size / PRINT_BYTES_PER_LINE;
        if(nlines * PRINT_BYTES_PER_LINE < size)
            nlines++;

        for(int line = 0; line < nlines; line++)
        {
            for(int j = 0; j < PRINT_BYTES_PER_LINE; j++)
            {
                if(offset + j >= size)
                    continue;
                else
                    snprintf(hexpair, sizeof(hexpair), fmt, data[offset + j]);

                out += hexpair;
            }

            offset += PRINT_BYTES_PER_LINE;

        }

        return (out);
    }

    void flush()
    {
        fflush(sys_);
        fflush(audit_);
        fflush(debug_);
    }

    /**
     * @return UTC-timestamp string in format
     * YYYYMMDD'T'HHmmSS'Z'
     */
    static std::string suffix()
    {
        char buf[128];
        time_t t = time(0);
        struct tm* day = gmtime(&t);
        snprintf(buf, 128, "%04d%02d%02dT%02d%02d%02dZ",
                    day->tm_year + 1900,
                    day->tm_mon + 1,
                    day->tm_mday,
                    day->tm_hour,
                    day->tm_min,
                    day->tm_sec);
       return std::string(buf);
    }

    /**
     * Initializes loggers
     * @return logger instance pointer, nullptr otherwise
     * @param params std::tuple with:
     * @tparam location (std::string)   -- path to log files, relative
     * @tparam app_name (std::string)   -- application name
     * @tparam suffix   (std::string)   -- suffix of log-file name
     * @tparam append   (bool)          -- append existing log file or not
     */
    bool init(std::tuple<path_t, std::string, std::string, bool> params)
    {
        RAISE_IF(initialized_, "tx.logger: Attempt to re-install");

        auto terminator = []
        {
            fprintf(stderr, log_t::init_error);
            throw std::runtime_error(log_t::init_error);
        };

        path_t location = std::get<0>(params);
        std::string app_name = std::get<1>(params);
        std::string sfx = std::get<2>(params);
        bool append = std::get<3>(params);

        // Check log location
        if (not location.exists())
        {
            fprintf(stderr,
                    "[tx.log.init] Log location '%s' doesn't exist\n",
                    location.path().c_str());
            terminator();
        }

        const char* file_open_mode = append ? "a" : "w+";
        std::string file_name = location.path() + "/" + app_name + "." + sfx + ".";

        std::string sys = file_name + "syslog";
        std::string audit = file_name + "audit";
        std::string debug = file_name + "debug";

        // Create or open syslog
        sys_ = fopen(sys.c_str(), file_open_mode);
        if (nullptr == sys_)
        {
            fprintf(stderr, "[tx.log.init] Can't create syslog file %s\n", sys.c_str());
            terminator();
        }
        LOGQ(LOG_TAG "init: Logger handler allocated");
        LOG(LOG_TAG "init: Syslog started, log file: %s", sys.c_str());

        // Create audit log if required by current logging level
        audit_ = fopen(audit.c_str(), file_open_mode);
        if (nullptr == audit_)
        {
            LOG_ERR(LOG_TAG "init: Can't create audit log file %s", audit.c_str());
            terminator();
        }
        LOG(LOG_TAG "init: Audit log started, log file: %s", audit.c_str());

        debug_ = fopen(debug.c_str(), file_open_mode);
        if (nullptr == debug_)
        {
            LOG_ERR(LOG_TAG "init: Can't create debug log file %s", debug.c_str());
            terminator();
        }
        LOG(LOG_TAG "init: Debug log started, log file: %s", debug.c_str());;
        initialized_ = true;

        return true;
    }

    /**
     * Shuts down logger
     */
    void shutdown()
    {
        if (nullptr != debug_)
        {
            LOGQ("[tx.log.shutdown] Debug log stopped");
            fclose(debug_);
        }

        if (nullptr != audit_)
        {
            LOGQ("[tx.log.shutdown] Audit log stopped");
            fclose(audit_);
        }

        if (nullptr != sys_)
        {
            LOGQ("[tx:logger:shutdown] Syslog stopped");
            fclose(sys_);
        }

    }

    /**
     * Logging routine
     * @param logfile Log file handle
     * @param prefix Log line prefix after timestamp
     * @param fmt Format string like used by sprintf()
     */
    template <typename ...Args>
    void logx(FILE* logfile, const char* prefix, const char* fmt, Args... args)
    {
        char head[64];

        // Prepare log record creation time
        timespec ts = { 0 };
        clock_gettime(CLOCK_REALTIME, &ts);

        // Print current local wall clock timestamp with microseconds into
        // temporary buffer looks like: 08:12:59.123456
        // or 22:12:59.123456 with right-padded with space
        uint pos = strftime(head, sizeof(head), "%Y-%m-%d %H:%M:%S", gmtime(&ts.tv_sec));
        pos += snprintf(head + pos, sizeof(head) - pos, ".%06luZ %s tid = %zd:%d ",
                        ts.tv_nsec / ns2us,
                        prefix,
                        syscall(SYS_gettid),
                        sched_getcpu());
        fprintf(logfile, fmt, head, args...);
    }

    /**
     * Console logging routine
     * @param prefix Log line prefix after timestamp
     * @param fmt Format string like used by sprintf()
     */
    template <typename ...Args>
    void to_stderr(const char* fmt, Args... args)
    { logx(stderr, "", fmt, args...); }

    FILE* sys_{ nullptr };
    FILE* audit_{ nullptr };
    FILE* debug_{ nullptr };

private:
    /// Default constructor disabled
    log_t() = default;

    /// Copy constructor disabled
    log_t(log_t const&) = delete;

    /// Assignment operator disabled
    log_t& operator=(const log_t&) = delete;

    bool initialized_{ false };

    /// Instance
    static log_t* instance_;
};

log_t* log_t::instance_{ nullptr };

#undef LOG_TAG
