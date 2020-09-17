#pragma once

#include <thread>

namespace stg::mdp::mdd::dissemination::thread
{

/**
 * @brief Enum for further execution
 */
enum class destruct_method{ join, detach };

/**
 * @class RAII thread channel class
 * 
 */
class thread_channel
{
public:

    thread_channel(std::thread th, destruct_method jd ):
                   method_(jd),
                   thread_(std::move(th))
    {}

    ~thread_channel()
    { cleanup(); }

    /**
     * @brief Return thread instance
     * 
     * @return std::thread& 
     */
    std::thread& get()
    { return (thread_); }

protected:

private:
    void cleanup()
    {
        if( thread_.joinable() )
        {
            if( method_ == destruct_method::join )
                thread_.join();
            else
                thread_.detach();
        }
    }

    destruct_method method_;
    std::thread thread_;

};



}