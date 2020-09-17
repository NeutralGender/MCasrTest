/**
 * \file time_barrier.hpp
 *
 * STG time barrier implementations
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

#include <sys/stat.h>

#include <string>
#include <boost/filesystem.hpp>
#include <boost/regex.hpp>

namespace stg::ai {

/// Path type
using path_type = std::string;

/// Filename type
using filename_type = std::string;

/// Wildcard regex
using wildcard_type = std::string;

/// File list (latency insensitive cases)
using filelist_type = std::list<path_type>;

/**
 * Path functionality wrapper
 */
struct path_t
{
    /**
     * Matches files in target directory by wildcard using regex
     * @param target Target directory for files lookup
     * @param expr Wildcard regex expression
     * @return Matched files names
     */
    static filelist_type ls_wildcard(path_type const& target, wildcard_type const& expr)
    {
        using boost::filesystem::directory_iterator;
        const boost::regex filter(expr);

        std::list<std::string> match;

        // Default ctor yields past-the-end
        directory_iterator end_it;
        for(directory_iterator it(target); it != end_it; ++it)
        {
            // Skip if not a file
        	const auto is_regular_file = boost::filesystem::is_regular_file(it->status());
            CONT_IF_NOT(is_regular_file);

            const auto fname = it->path().filename().string();
            const auto lookup_result = boost::regex_search(fname, filter);
            CONT_IF_NOT(lookup_result);

            // File matches, store it
            match.push_back(fname);
        }

        return match;
    }

    /// Constructs using given path
    path_t(path_type const& path):
        path_( path )
    {}

    /// Checks existence of given path
    bool exists() const
    {
        struct stat info;

        if(stat(path_.c_str(), &info) != 0)
            return false;
        else if (info.st_mode & S_IFDIR)
            return true;
        else
            return false;
    }

    /// Provides path content
    path_type const& path() const
    { return path_; }

private:
    /// Path name
    const path_type path_;
};

/**
 * File functionality wrapper
 */
struct file_t
{
    /// Constructs using given filename
    file_t(filename_type const& filename):
        fname_( filename )
    {}

    /// Checks existence of given file
    bool exists()
    {
        struct stat info;

        if(stat(fname_.c_str(), &info) != 0)
            return false;
        else if (info.st_mode & (S_IFREG | S_IFIFO))
            return true;
        else
            return false;
    }

    /// Fetch file name
    filename_type name() const
    { return fname_; }

private:
    /// Default construction was disabled
    file_t() = delete;

    /// Stores file name
    filename_type fname_;
};

}  // namespace stg::ai
