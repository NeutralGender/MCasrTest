#pragma once

#include <vector>

#include <stg/ai/xring/xring.hpp>
#include <stg/mdp/mdd/dissemination/server/dissamination/dissamination.hpp>
#include <stg/mdp/mdd/conf/dissemination/types.hpp>
#include <stg/mdp/mdd/dissemination/thread/thread_channel.hpp>
#include <stg/mdp/mdd/dissemination/cpu_affinity/cpu_affinity.hpp>
#include <stg/mdp/mdd/common/reader/reader.hpp>
#include <stg/mdp/mdd/dissemination/server/producer/producer.hpp>
#include <stg/mdp/mdd/dissemination/server/consm/consumer.hpp>


namespace stg::mdp::mdd::dissemination::server
{

using stg::ai::xring::xring_t;
using stg::ai::ai_wait_t;
using stg::mdp::mdd::dissm_server::dissm_sender;

using stg::mdp::mdd::dissm_server::dissm_sender;
using stg::mdp::mdd::conf::dissemination::channel_t;
using stg::mdp::mdd::dissemination::thread::thread_channel;
using stg::mdp::mdd::dissemination::thread::destruct_method;
using stg::mdp::mdd::dissemination::cpu_affinity::cpu_affinity_t;
using stg::mdp::mdd::common::reader::reader_t;
using stg::mdp::mdd::server::prod::producer;
using stg::mdp::mdd::dissemination::server::consm::consumer;

typedef void (*consm_func_t)(xring_t<>& rng, stg::ai::ai_wait_t& sem, std::size_t& _consume_ix_, std::size_t cores, std::size_t threads);
typedef void (*prod_func_t)(xring_t<>& rng, stg::ai::ai_wait_t& sem);

/**
 * @brief Server instance
 * 
 */
class server_t
{
public:
    server_t(const std::vector<channel_t>& bconf, const std::size_t delay):
             feeds(bconf.size()),
             //consm_threads_(bconf.size()),
             prod_threads_(1)
    {
        for( std::size_t i = 0; i < feeds.size(); ++i )
        {
            // Init feed, dissm_sender contains all feed`s channels
            feeds[i] = std::make_unique<dissm_sender>(bconf[i], delay);
        }
    }

    ~server_t()
    {}

    /**
     * @brief Initialize multicast feed`s channels
     */
    [[nodiscard]] bool init_sender()
    {
        for( auto& element : feeds )
        {
            if( not element->mcast_init() )
                return false;
        }

        return true;
    }

    /**
     * @brief Create a consm thread object
     * 
     * @param th_func pointer to execution function
     * @param rng XRING instance
     * @param sem wait handle
     */
    void create_consm_thread( //consm_func_t th_func, 
                              xring_t<>& rng, 
                              stg::ai::ai_wait_t& sem,
                              std::atomic<std::size_t>& _consume_ix_,
                              std::size_t cores,
                              std::size_t threads)
    {
        //std::cout << consm_threads_.size() << std::endl;

        for( auto& feed : feeds )
            feed->start_channels( rng,  sem, _consume_ix_, cores, threads );

    /*
        for( std::size_t i = 0; i < consm_threads_.size(); ++i )
        {
            consm_threads_[i] = std::make_unique<thread_channel>( std::thread(th_func,
                                                                               std::ref(rng),
                                                                               std::ref(sem),
                                                                               std::ref(feeds[i]),
                                                                               std::ref(_consume_ix_) ),
                                                                   destruct_method::join);

            m_cpu_aff.set_affinity( i, consm_threads_[i]->get().native_handle() );
            m_cpu_aff.getcpu();
        }
    */

    }

    
    void send_without_ring(stg::ai::ai_wait_t& sem,
                           std::atomic<std::size_t>& _consume_ix_, 
                           const std::string& filename,
                           double& time,
                           std::size_t loop,
                           std::size_t cores,
                           std::size_t threads,
                           std::size_t package)
    {
        for( auto& feed : feeds )
            feed->channels_without_ring(  sem, _consume_ix_, filename, time, loop, cores, threads, package );
    }

    void send_without_ring_micro(stg::ai::ai_wait_t& sem,
                                 std::atomic<std::size_t>& _consume_ix_, 
                                 const std::string& filename,
                                 double& time,
                                 std::size_t loop,
                                 std::size_t cores,
                                 std::size_t threads,
                                 std::size_t package)
    {
        for( auto& feed : feeds )
            feed->channels_without_ring_micro(  sem, _consume_ix_, filename, time, loop, cores, threads, package );
    }

    /**
     * @brief Create a prod thread object
     * 
     * @param th_func pointer to execution function
     * @param rng XRING instance
     * @param sem wait handle
     */
    void create_prod_thread( prod_func_t th_func, xring_t<>& rng, stg::ai::ai_wait_t& sem )
    {
        for( std::size_t i = 0; i < prod_threads_.size(); ++i )
        {
            prod_threads_[i] = std::make_unique<thread_channel>( std::thread(th_func,
                                                                              std::ref(rng),
                                                                              std::ref(sem)),
                                                                  destruct_method::join );
        }
    }

    template <typename T>
    void test_create_prod_thread( xring_t<>& rng,
                                  stg::ai::ai_wait_t& sem,
                                  reader_t<T>& reader,
                                  std::size_t loop,
                                  const std::string& dump )
    {
        for( std::size_t i = 0; i < prod_threads_.size(); ++i )
        {
            prod_threads_[i] = std::make_unique<thread_channel>( std::thread(producer::test_produce<T>,
                                                                              std::ref(rng),
                                                                              std::ref(sem),
                                                                              std::ref(reader),
                                                                              loop, dump),
                                                                  destruct_method::join );
        }
    }

protected:

private:

    /**
     * Pointer to one feed(A)
     */
    std::vector< std::unique_ptr<dissm_sender> > feeds;

    //std::vector< std::unique_ptr<thread_channel> > consm_threads_;
    std::vector< std::unique_ptr<thread_channel> > prod_threads_;
    //cpu_affinity_t m_cpu_aff;
};

}
