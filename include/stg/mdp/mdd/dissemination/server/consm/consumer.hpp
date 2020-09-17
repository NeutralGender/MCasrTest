#pragma once

#include <string_view>
#include <string>
#include <vector>
#include <optional>
#include <mutex>
#include <atomic>
#include <chrono>

#include <stg/ai/xring/xring.hpp>
#include <stg/ai/mock.hpp>
#include <stg/ai/semaphore.hpp>

#include <stg/mdp/mdd/dissemination/server/dissamination/dissamination.hpp>
#include <stg/mdp/mdd/dissemination/server/types.hpp>
#include <stg/mdp/mdd/net/multicast/send.hpp>

#include <stg/mdp/mdd/common/reader/reader.hpp>
#include <stg/mdp/mdd/common/reader/pcap/pcap_reader.hpp>

namespace stg::mdp::mdd::dissemination::server::consm
{

using stg::ai::xring::xring_t;
using stg::ai::xring::xring_fetch_t;
using stg::ai::mock;
using stg::ai::ai_wait_t;
//using stg::mdp::mdd::dissm_server::dissm_sender;
using stg::mdp::mdd::server::diss_mock;
using stg::mdp::mdd::server::test_diss_mock;
using stg::mdp::mdd::net::multicast::mcast_sender;

using stg::mdp::mdd::common::reader::reader_t;
using stg::mdp::mdd::common::reader::pcap::pcap_reader;


/**
 * @brief Consumer XRING class
 */
class consumer
{
public:
    consumer()
    {}

    ~consumer()
    {}

    /**
     * @brief Reading data from xring 
     * 
     * @param rng xring instance
     * @param sem wait handler
     * @param sender sender dissemination
     */
    //static void fetch_data( xring_t<>& rng, stg::ai::ai_wait_t& sem, std::unique_ptr<dissm_sender>& sender );

    //template<typename Iterator>
    static void test_fetch_data( xring_t<>& rng, 
                                 stg::ai::ai_wait_t& sem, 
                                 std::atomic<std::size_t>& _consume_ix_,
                                 std::vector< std::unique_ptr<mcast_sender> >&& begin,
                                 std::mutex& mtx,
                                 std::size_t delay,
                                 std::size_t i );

    static void test_fetch_data_without( stg::ai::ai_wait_t& sem, 
                                         std::atomic<std::size_t>& _consume_ix_,
                                         std::vector< std::unique_ptr<mcast_sender> >&& channels,
                                         std::mutex& mtx,
                                         const std::string& filename,
                                         std::size_t loop,
                                         std::size_t delay,
                                         double&  i,
                                         std::size_t package );

    static void test_fetch_data_without_micro( stg::ai::ai_wait_t& sem, 
                                               std::atomic<std::size_t>& _consume_ix_,
                                               std::vector< std::unique_ptr<mcast_sender> >&& channels,
                                               std::mutex& mtx,
                                               const std::string& filename,
                                               std::size_t loop,
                                               std::size_t delay,
                                               double&  i,
                                               std::size_t package );

private:
    /**
     * @brief thread local index
     */
    static thread_local unsigned _consume_ix;
};

thread_local unsigned consumer::_consume_ix = 0;

/*
void consumer::fetch_data( xring_t<>& rng, stg::ai::ai_wait_t& sem, std::unique_ptr<dissm_sender>& sender )
{
    #define LOG_TAG "[stg.ai.test.xring] "

    LOG_CON(LOG_TAG "%s", "Consumer IN\n");
    thread_local auto &fetcher_reader = rng.alloc_fetcher("");

    while( _consume_ix < 10 )
    {
        for( ; _consume_ix < 10; ++_consume_ix )
        {
            auto [ instance, err_pop ] = fetcher_reader.pop< diss_mock >();
            if( instance == nullptr ) // No data for fetching
            {
                sem.wake();
                break;
            }

            LOG_CON(LOG_TAG "Consumer: %d\n", _consume_ix);
            
            
            // Bad, but for test only
            sender->send_channel( instance->tag, std::to_string(instance->seq) + instance->value );
        }
    }
    sem.wake();
    LOG_CON(LOG_TAG "%s", "Consumer Out\n");

    #undef LOG_TAG
}
*/

//template<typename Iterator>
void consumer::test_fetch_data( xring_t<>& rng, 
                                stg::ai::ai_wait_t& sem, 
                                std::atomic<std::size_t>& _consume_ix_,
                                std::vector< std::unique_ptr<mcast_sender> >&& channels,
                                std::mutex& mtx,
                                std::size_t delay,
                                std::size_t i )
{
    //LOG_CON(LOG_TAG "%s", "Consumer IN\n");
    thread_local auto &fetcher_reader = rng.alloc_fetcher("");

    bool breake = false;

    _consume_ix_ = 0;

    std::atomic<std::size_t> atom = 0;

    for(auto& n : channels)
    {
        if( n == nullptr )
        {
            std::cout << "i: " << i << std::endl;
            std::cin.get();
        }
    }

    auto start_time = std::chrono::system_clock::now();
    while( not breake )
    {
        while( true )
        {
            //std::unique_lock<std::mutex> lock(mtx);
            auto [ instance, err_pop ] = fetcher_reader.pop< test_diss_mock >();
            if( instance == nullptr ) // No data for fetching
            {
                sem.wake();
                break;
            }

            //LOG_CON(LOG_TAG "Consumer:%i \n", _consume_ix);
            
            if( instance->caplen == 4 )
            {
                breake = true;
                std::cout << "Buf: " << strlen((char*)instance->value) << instance->value << std::endl;
                
                for(std::size_t i = 0; i< 100; ++i)
                    channels[0]->send((char*)instance->value);

                break;
            }

            // Bad, but for test only
            std::unique_lock<std::mutex> lock(mtx);
            for(auto& ch : channels)
            {
                _consume_ix_++;
                atom++;

                //usleep(delay);
                std::this_thread::sleep_for(std::chrono::microseconds(delay));
                ch->send((char*)instance->value);
            }

            //_consume_ix_++;
        }
    }

    auto end_time = std::chrono::system_clock::now();
    std::chrono::duration<double, std::milli> fp_ms = end_time - start_time;
    //std::cout << "CTest: " << fp_ms.count() << " ms with " << _consume_ix_ << " packages" << std::endl;
    std::cout << "CTest: " << fp_ms.count() << " ms with " << atom << " packages" << std::endl;

    sem.wake();
    //LOG_CON(LOG_TAG "%s", "Consumer Out\n");
}


void consumer::test_fetch_data_without( stg::ai::ai_wait_t& sem, 
                                        std::atomic<std::size_t>& _consume_ix_,
                                        std::vector< std::unique_ptr<mcast_sender> >&& channels,
                                        std::mutex& mtx,
                                        const std::string& filename,
                                        std::size_t loop,
                                        std::size_t delay,
                                        double& time,
                                        std::size_t package )
{
    bool breake = false;
    std::atomic<std::size_t> atom = 0;

    reader_t<pcap_reader> reader( *(new pcap_reader) );
    reader.open(filename);
        
    //std::vector<u_char*> v;
    std::vector<std::string> v;
    v.reserve(1000);

    while(true)
    {
        auto[caplen, pack_data] = reader.read();
            
        if(caplen == -2)
        {
            break;
        }

        v.push_back(std::string{(char*)pack_data, caplen});
    }

    auto start_time = std::chrono::system_clock::now();

/*
    for( std::size_t i = 0; i < loop; ++i )
    {
        for(auto k = 0; k < v.size(); ++k)
        {
            for(auto& ch : channels)
            {
                ++atom;
                ++_consume_ix_;
                ch->send(v[k].c_str());
            }
            std::this_thread::sleep_for(std::chrono::microseconds(delay));
        }
    }
*/

    for(std::size_t i = 0; i < loop; ++i)
    {
        for(auto k = 0; k < v.size(); k+=package)
        {
            //std::size_t current_p = k;
            for(auto& ch : channels)
            {
                for(auto p = k; p < (k + package) && (p != v.size()); ++p)
                {
                    ++atom;
                    ++_consume_ix_;
                    ch->send(v[p].c_str());
                }
            }
            std::this_thread::sleep_for(std::chrono::microseconds(delay));
        }
    }

    auto end_time = std::chrono::system_clock::now();
    std::chrono::duration<double, std::milli> fp_ms = end_time - start_time;
    std::unique_lock<std::mutex> ulock(mtx);
    std::cout << "CTest: " << fp_ms.count() << " ms with " << atom << " packages" << std::endl;
    time += fp_ms.count();
    ulock.unlock();
/*
    auto start_time = std::chrono::system_clock::now();
    while( not breake )
    {
        while( true )
        {
            auto [ instance, err_pop ] = fetcher_reader.pop< test_diss_mock >();
            if( instance == nullptr ) // No data for fetching
            {
                sem.wake();
                break;
            }
            
            // Bad, but for test only
            std::unique_lock<std::mutex> lock(mtx);
            for(auto& ch : channels)
            {
                _consume_ix_++;
                atom++;

                //usleep(delay);
                std::this_thread::sleep_for(std::chrono::microseconds(delay));
                ch->send((char*)instance->value);
            }
        }
    }
*/
    //auto end_time = std::chrono::system_clock::now();
    //std::chrono::duration<double, std::milli> fp_ms = end_time - start_time;
    //std::cout << "CTest: " << fp_ms.count() << " ms with " << atom << " packages" << std::endl;

    sem.wake();
}

void consumer::test_fetch_data_without_micro( stg::ai::ai_wait_t& sem, 
                                              std::atomic<std::size_t>& _consume_ix_,
                                              std::vector< std::unique_ptr<mcast_sender> >&& channels,
                                              std::mutex& mtx,
                                              const std::string& filename,
                                              std::size_t loop,
                                              std::size_t delay,
                                              double& time,
                                              std::size_t package )
{
    bool breake = false;
    std::atomic<std::size_t> atom = 0;

    reader_t<pcap_reader> reader( *(new pcap_reader) );
    reader.open(filename);
        
    std::vector<std::string> v;
    v.reserve(1000);

    while(true)
    {
        auto[caplen, pack_data] = reader.read();
            
        if(caplen == -2)
        {
            break;
        }

        v.push_back(std::string{(char*)pack_data, caplen});
    }

    auto start_time = std::chrono::system_clock::now();

    for( std::size_t i = 0; i < loop; ++i )
    {
        for(auto k = 0; k < v.size(); k+=package)
        {
            std::size_t current_p = k;
            for(auto& ch : channels)
            {
                for(auto p = current_p; p < (current_p + package) && (p != v.size()); ++p)
                {
                    ++atom;
                    ++_consume_ix_;
                    ch->send(v[p].c_str());
                }
                //std::cout << std::this_thread::get_id() << " " << atom << std::endl;
                //atom = 0;
            }
            for( std::size_t i = 0; i < delay; ++i )
            { }
        }
    }

    auto end_time = std::chrono::system_clock::now();
    std::chrono::duration<double, std::milli> fp_ms = end_time - start_time;
    std::unique_lock<std::mutex> ulock(mtx);
    std::cout << "CTest: " << fp_ms.count() << " ms with " << atom << " packages" << std::endl;
    time += fp_ms.count();
    ulock.unlock();

    sem.wake();
}


}