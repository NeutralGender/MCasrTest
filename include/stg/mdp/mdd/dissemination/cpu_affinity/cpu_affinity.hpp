#pragma once

#include <iostream>
#include <cstddef>
#include <thread>
#include <pthread.h>
#include <sched.h>

#include <stg/ai/log.hpp>

namespace stg::mdp::mdd::dissemination::cpu_affinity
{

#define LOG_TAG "[stg.ai.test.xring] "

/**
 * @brief Class for setting processor affinity
 * 
 */
class cpu_affinity_t
{
public:
    cpu_affinity_t()
    { CPU_ZERO(&cpuset_); }

    ~cpu_affinity_t()
    {}

    /**
     * @brief Set the affinity object
     * 
     * @param ix CPU index
     * @param th_handle thread handle
     */
    void set_affinity(const std::size_t ix, std::thread::native_handle_type th_handle )
    {
        cpu_set(ix);
        cpu_set_affinity(th_handle);
    }

    void get_affinity(std::thread::native_handle_type th_handle)
    {
        int ret = -1;
        if( (ret = pthread_getaffinity_np(th_handle, sizeof(cpu_set_t), &cpuset_)) != 0 )
            LOG_CON(LOG_TAG "pthread_getaffinity_np error: %d", ret);

        for(int j = 0; j < CPU_SETSIZE; j++)
            if(CPU_ISSET(j, &cpuset_))
                std::cout << j << " ";
        
        std::cout << std::endl;
    }

    /**
     * @brief Determine CPU on which the calling thread is running
     */
    void getcpu()
    { LOG_CON(LOG_TAG "%d", sched_getcpu()); }

protected:

private:
    /**
     * @brief Add CPU to cpusetp
     * 
     * @param ix CPU index
     */
    void cpu_set(const std::size_t ix)
    { CPU_SET(ix, &cpuset_); }

    /**
     * @brief Set CPU affinity mask of the thread to the CPU
     * 
     * @param th_handle Thead handle
     */
    void cpu_set_affinity( std::thread::native_handle_type th_handle )
    { 
        int ret = -1;
        if( ( ret = pthread_setaffinity_np(th_handle, sizeof(cpu_set_t), &cpuset_) ) != 0)
            LOG_CON(LOG_TAG "pthread_setaffinity_np error: %d", ret);
    }

    /**
     * @brief Data structure represents a set of CPUs
     */
    cpu_set_t cpuset_;
};

}