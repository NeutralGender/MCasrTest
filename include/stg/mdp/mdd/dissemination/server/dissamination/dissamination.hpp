#pragma once

#include <vector>
#include <string>
#include <thread>
#include <mutex>

#include <stg/ai/xring/xring.hpp>
#include <stg/ai/manage.hpp>
#include <stg/mdp/mdd/net/multicast/send.hpp>
#include <stg/mdp/mdd/conf/dissemination/types.hpp>
#include <stg/mdp/mdd/dissemination/thread/thread_channel.hpp>
#include <stg/mdp/mdd/dissemination/cpu_affinity/cpu_affinity.hpp>
#include <stg/mdp/mdd/dissemination/server/consm/consumer.hpp>


// FOR @maximus
#include <unistd.h>
// 

namespace stg::mdp::mdd::dissm_server
{

using stg::ai::xring::xring_t;
using stg::ai::ai_wait_t;
using stg::mdp::mdd::net::multicast::mcast_sender;
using stg::mdp::mdd::conf::dissemination::channel_t;
using stg::mdp::mdd::conf::dissemination::production;
using stg::mdp::mdd::dissemination::thread::thread_channel;
using stg::mdp::mdd::dissemination::thread::destruct_method;
using stg::mdp::mdd::dissemination::cpu_affinity::cpu_affinity_t;
using stg::mdp::mdd::dissemination::server::consm::consumer;


typedef void (*consm_func_t)(xring_t<>& rng, stg::ai::ai_wait_t& sem, std::size_t& _consume_ix_, std::size_t cores, std::size_t threads);


/**
 * @brief Server feed channel class 
 */
class dissm_sender
{
public:
    dissm_sender(const channel_t& dconf, const std::size_t delay):
                 channels_( dconf.broadcast.size() ),
                 //consm_threads_( dconf.broadcast.size() ),
                 broadcast_( dconf.broadcast ),
                 delay(delay)
    {
        for( std::size_t i = 0; i < channels_.size(); i++ )
            channels_[i] = std::make_unique<mcast_sender>(dconf.src, dconf.broadcast[i]);
    }

    ~dissm_sender()
    {}

    /**
     * @brief initialize multicast channels
     * 
     */
    [[nodiscard]] bool mcast_init()
    {
        for( auto& channel : channels_ )
        {
            if( not channel->create_socket(SOCK_DGRAM) )
                return false;

            channel->init_socket();

            if( not channel->socket_settings() )
                return false;
        }

        return true;
    }

    void start_channels(xring_t<>& rng, 
                        stg::ai::ai_wait_t& sem,
                        std::atomic<std::size_t>& _consume_ix_,
                        std::size_t cores,
                        std::size_t threads)
    {
        static std::size_t cur = 0;
        static std::size_t i = 0;
        std::size_t groups = channels_.size();
        std::size_t groups_in_thread = ( groups / (cores * threads) );

        consm_threads_.resize( cores * threads );

        for(std::size_t core = 0; core < cores; ++core)
        {
            for(std::size_t thread = 0; thread < threads; ++thread)
            {
                cpu_affinity_t m_cpu_aff;

                //cur += groups_in_thread;
                if( ( groups - ( (core + 1 ) * ( thread + 1 ) * groups_in_thread ) ) >= groups_in_thread )
                {
                    std::vector< std::unique_ptr<mcast_sender> > sub (std::make_move_iterator(channels_.begin()+cur), std::make_move_iterator(channels_.begin()+cur+groups_in_thread));
                    
                    for(auto& n : sub)
                    {
                        if( n == nullptr )
                            std::cout << "core: " << core << " thread: " << thread << " cur: " << cur << std::endl;
                    }
                    
                    consm_threads_[i] = std::make_unique<thread_channel>( std::thread( consumer::test_fetch_data,
                                                                                        std::ref(rng),
                                                                                        std::ref(sem),
                                                                                        std::ref(_consume_ix_),
                                                                                        std::move(sub), 
                                                                                        std::ref(mtx),
                                                                                        delay,
                                                                                        i ),
                                                                          destruct_method::join);
                
                    cur += groups_in_thread;
                    m_cpu_aff.set_affinity( core+1, consm_threads_[i]->get().native_handle() );
                    m_cpu_aff.getcpu();
                    m_cpu_aff.get_affinity(consm_threads_[i]->get().native_handle());
                    i++;
                }
            
                else
                {
                    std::vector< std::unique_ptr<mcast_sender> > sub (std::make_move_iterator(channels_.begin()+cur), std::make_move_iterator(channels_.end()));
                 
                    consm_threads_[i] = std::make_unique<thread_channel>( std::thread( consumer::test_fetch_data,
                                                                                       std::ref(rng),
                                                                                       std::ref(sem),
                                                                                       std::ref(_consume_ix_),
                                                                                       std::move(sub), 
                                                                                       std::ref(mtx),
                                                                                       delay,
                                                                                       i ),
                                                                          destruct_method::join);
                
                
                    m_cpu_aff.set_affinity( core+1, consm_threads_[i]->get().native_handle() );
                    m_cpu_aff.getcpu();
                    m_cpu_aff.get_affinity(consm_threads_[i]->get().native_handle());
                }
            
            }
        }

    /*
        std::cout << "channels_.size(): " << channels_.size() << std::endl;
        for( std::size_t i = 0; i < channels_.size(); ++i )
        {
            consm_threads_[i] = std::make_unique<thread_channel>( std::thread( consumer::test_fetch_data,
                                                                               std::ref(rng),
                                                                               std::ref(sem),
                                                                               std::ref(_consume_ix_) ),
                                                                   destruct_method::join);

            m_cpu_aff.set_affinity( i%3, consm_threads_[i]->get().native_handle() );
            m_cpu_aff.getcpu();
        }
    */

    }

    void channels_without_ring(stg::ai::ai_wait_t& sem,
                               std::atomic<std::size_t>& _consume_ix_,
                               const std::string& filename,
                               double& time,
                               std::size_t loop,
                               std::size_t cores,
                               std::size_t threads,
                               std::size_t package,
                               std::size_t package_per_tact,
                               std::size_t tact_us)
    {
        static std::size_t cur = 0;
        static std::size_t i = 0;
        std::size_t groups = channels_.size();
        std::size_t all = cores * threads;
        std::size_t core = 0;
        std::size_t groups_in_thread = ( groups / (cores * threads) ) > 0 ? 
                                            ( groups / (cores * threads) ) : 1;

        consm_threads_.resize( cores * threads );

    /*
        while( (all - 1) > i || ( groups - 1 ) < cur )
        {
            cpu_affinity_t m_cpu_aff;

            std::vector< std::unique_ptr<mcast_sender> > sub (std::make_move_iterator(channels_.begin()+cur), std::make_move_iterator(channels_.begin()+cur+groups_in_thread));
                                        
            consm_threads_[i] = std::make_unique<thread_channel>( std::thread( consumer::test_fetch_data_without,
                                                                                std::ref(sem),
                                                                                std::ref(_consume_ix_),
                                                                                std::move(sub), 
                                                                                std::ref(mtx),
                                                                                std::ref(filename),
                                                                                loop,
                                                                                delay,
                                                                                std::ref(time) ),
                                                                    destruct_method::join);
                
            cur += groups_in_thread;
            m_cpu_aff.set_affinity( core, consm_threads_[i]->get().native_handle() );
            m_cpu_aff.getcpu();
            m_cpu_aff.get_affinity(consm_threads_[i]->get().native_handle());
            i++;
            if(core < ( cores - 1 ) )
                core++;
        }

        cpu_affinity_t m_cpu_aff;

        std::vector< std::unique_ptr<mcast_sender> > sub (std::make_move_iterator(channels_.begin()+cur), std::make_move_iterator(channels_.end()));
                 
        consm_threads_[i] = std::make_unique<thread_channel>( std::thread( consumer::test_fetch_data_without,
                                                                            std::ref(sem),
                                                                            std::ref(_consume_ix_),
                                                                            std::move(sub), 
                                                                            std::ref(mtx),
                                                                            std::ref(filename),
                                                                            loop,
                                                                            delay,
                                                                            std::ref(time) ),
                                                                destruct_method::join);
                
                
        m_cpu_aff.set_affinity( cores, consm_threads_[i]->get().native_handle() );
        m_cpu_aff.getcpu();
        m_cpu_aff.get_affinity(consm_threads_[i]->get().native_handle());
    */

    
        for(std::size_t core = 0; core < cores; ++core)
        {
            for(std::size_t thread = 0; thread < threads; ++thread)
            {
                cpu_affinity_t m_cpu_aff;

                //cur += groups_in_thread;
                //if( ( groups - ( (core + 1 ) * ( thread + 1 ) * groups_in_thread ) ) >= groups_in_thread )
                if( (all - 1) > i && ( groups - 1 ) > cur )
                {
                    std::vector< std::unique_ptr<mcast_sender> > sub (std::make_move_iterator(channels_.begin()+cur), std::make_move_iterator(channels_.begin()+cur+groups_in_thread));
                                        
                    consm_threads_[i] = std::make_unique<thread_channel>( std::thread( consumer::test_fetch_data_without,
                                                                                        std::ref(sem),
                                                                                        std::ref(_consume_ix_),
                                                                                        std::move(sub), 
                                                                                        std::ref(mtx),
                                                                                        std::ref(filename),
                                                                                        loop,
                                                                                        delay,
                                                                                        std::ref(time),
                                                                                        package,
                                                                                        package_per_tact,
                                                                                        tact_us ),
                                                                          destruct_method::join);
                
                    cur += groups_in_thread;
                    m_cpu_aff.set_affinity( core, consm_threads_[i]->get().native_handle() );
                    m_cpu_aff.getcpu();
                    m_cpu_aff.get_affinity(consm_threads_[i]->get().native_handle());
                    i++;
                }
            
                else
                {
                    std::vector< std::unique_ptr<mcast_sender> > sub (std::make_move_iterator(channels_.begin()+cur), std::make_move_iterator(channels_.end()));
                 
                    consm_threads_[i] = std::make_unique<thread_channel>( std::thread( consumer::test_fetch_data_without,
                                                                                       std::ref(sem),
                                                                                       std::ref(_consume_ix_),
                                                                                       std::move(sub), 
                                                                                       std::ref(mtx),
                                                                                       std::ref(filename),
                                                                                       loop,
                                                                                       delay,
                                                                                       std::ref(time),
                                                                                       package,
                                                                                       package_per_tact,
                                                                                       tact_us ),
                                                                          destruct_method::join);
                
                
                    m_cpu_aff.set_affinity( core, consm_threads_[i]->get().native_handle() );
                    m_cpu_aff.getcpu();
                    m_cpu_aff.get_affinity(consm_threads_[i]->get().native_handle());

                    return;
                }
            
            }
        }

    

    }

    void channels_without_ring_micro(stg::ai::ai_wait_t& sem,
                                     std::atomic<std::size_t>& _consume_ix_,
                                     const std::string& filename,
                                     double& time,
                                     std::size_t loop,
                                     std::size_t cores,
                                     std::size_t threads,
                                     std::size_t package,
                                     std::size_t package_per_tact,
                                     std::size_t tact_us)
    {
        static std::size_t cur = 0;
        static std::size_t i = 0;
        std::size_t groups = channels_.size();
        std::size_t all = cores * threads;
        std::size_t core = 0;
        std::size_t groups_in_thread = ( groups / (cores * threads) ) > 0 ? 
                                            ( groups / (cores * threads) ) : 1;

        consm_threads_.resize( cores * threads );

        for(std::size_t core = 0; core < cores; ++core)
        {
            for(std::size_t thread = 0; thread < threads; ++thread)
            {
                cpu_affinity_t m_cpu_aff;

                //cur += groups_in_thread;
                //if( ( groups - ( (core + 1 ) * ( thread + 1 ) * groups_in_thread ) ) >= groups_in_thread )
                if( (all - 1) > i && ( groups - 1 ) > cur )
                {
                    std::vector< std::unique_ptr<mcast_sender> > sub (std::make_move_iterator(channels_.begin()+cur), std::make_move_iterator(channels_.begin()+cur+groups_in_thread));
                                        
                    consm_threads_[i] = std::make_unique<thread_channel>( std::thread( consumer::test_fetch_data_without_micro,
                                                                                        std::ref(sem),
                                                                                        std::ref(_consume_ix_),
                                                                                        std::move(sub), 
                                                                                        std::ref(mtx),
                                                                                        std::ref(filename),
                                                                                        loop,
                                                                                        delay,
                                                                                        std::ref(time),
                                                                                        package,
                                                                                        package_per_tact,
                                                                                        tact_us ),
                                                                          destruct_method::join);
                
                    cur += groups_in_thread;
                    m_cpu_aff.set_affinity( core, consm_threads_[i]->get().native_handle() );
                    m_cpu_aff.getcpu();
                    m_cpu_aff.get_affinity(consm_threads_[i]->get().native_handle());
                    i++;
                }
            
                else
                {
                    std::vector< std::unique_ptr<mcast_sender> > sub (std::make_move_iterator(channels_.begin()+cur), std::make_move_iterator(channels_.end()));
                 
                    consm_threads_[i] = std::make_unique<thread_channel>( std::thread( consumer::test_fetch_data_without_micro,
                                                                                       std::ref(sem),
                                                                                       std::ref(_consume_ix_),
                                                                                       std::move(sub), 
                                                                                       std::ref(mtx),
                                                                                       std::ref(filename),
                                                                                       loop,
                                                                                       delay,
                                                                                       std::ref(time),
                                                                                       package,
                                                                                       package_per_tact,
                                                                                       tact_us ),
                                                                          destruct_method::join);
                
                
                    m_cpu_aff.set_affinity( core, consm_threads_[i]->get().native_handle() );
                    m_cpu_aff.getcpu();
                    m_cpu_aff.get_affinity(consm_threads_[i]->get().native_handle());

                    return;
                }
            
            }
        }

    }

    /**
     * @brief Check tag and send by tag`s multicast channel
     * 
     * @param tag multicsat channel tag
     * @param message message
     */
    void send_channel( const std::string& tag, const std::string& message )
    {
        for( std::size_t i = 0; i < channels_.size(); ++i )
        {
            if( broadcast_[i].tag == tag )
                channels_[i]->send(message);
        }
    }

    void test_send_channel( const std::string& message )
    {
        // FOR @maximus
            usleep(delay); // 20 millisec
        // 
        for(auto& ch : channels_ )
            ch->send(message);
    }

protected:

private:
    std::vector< std::unique_ptr<mcast_sender> > channels_;
    std::vector< std::unique_ptr<thread_channel> > consm_threads_;
    //cpu_affinity_t m_cpu_aff;
    std::vector< production > broadcast_;
    const std::size_t delay;
    std::mutex mtx;
};


}
