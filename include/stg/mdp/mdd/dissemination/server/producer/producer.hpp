#pragma once

#include <string>
#include <cstddef>
#include <map>

#include <stg/mdp/mdd/dissemination/server/types.hpp>
#include <stg/ai/xring/xring.hpp>
#include <stg/ai/semaphore.hpp>
#include <stg/mdp/mdd/common/reader/reader.hpp>
#include <stg/mdp/mdd/common/reader/pcap/pcap_reader.hpp>

namespace stg::mdp::mdd::server::prod
{

using stg::mdp::mdd::server::diss_mock;
using stg::mdp::mdd::server::addr_data;
using stg::mdp::mdd::server::test_diss_mock;
using stg::ai::xring::xring_t;
using stg::ai::xring::xslot_t;
using stg::ai::ai_wait_t;
using stg::mdp::mdd::common::reader::reader_t;

/**
 * @brief XRING producer
 * 
 */
class producer
{    
public:
    producer()
    {}

    ~producer()
    {}

    /**
     * @brief Write data
     * 
     * @param rng XRING instance
     * @param sem wait handler
     */
    static void produce( xring_t<>& rng, stg::ai::ai_wait_t& sem );

    /**
     * @brief Test XRING producer
     * 
     * @tparam T reader low-level module
     * @param rng XRING instance
     * @param sem wait handler
     * @param reader multicast captured data
     */
    template <typename T>
    static void test_produce(xring_t<>& rng, 
                             stg::ai::ai_wait_t& sem, 
                             reader_t<T>& reader,
                             std::size_t loop, const std::string& dump);

protected:


private:
    static std::size_t ix;
};

std::size_t producer::ix = 0;

void producer::produce(xring_t<>& rng, stg::ai::ai_wait_t& sem)
{
    #define LOG_TAG "[stg.ai.test.xring] "
    diss_mock m;

    sem.wait();
    for( std::size_t i = 0; i < 10; ++i )
    {
        LOG_CON(LOG_TAG "Produce: %d\n", i);

        m.value.resize( std::rand() % 50 );

        auto instance = rng.alloc< diss_mock >( sizeof( m ) );

        instance->value.resize( std::rand() % 50 );

        instance->seq = i;
        instance->tag = ( i % 6 ) + '0';

        std::fill( instance->value.begin(), instance->value.end(), i + '0' );
            
        auto slot = xslot_t::slot(instance);

        slot->commit();

        sem.wait();
    }
    LOG_CON(LOG_TAG "%s", "Producer Out\n");

    #undef LOG_TAG
}

template <typename T>
void producer::test_produce( xring_t<>& rng, 
                             stg::ai::ai_wait_t& sem, 
                             reader_t<T>& reader, 
                             std::size_t loop, const std::string& dump )
{
    #define LOG_TAG "[stg.ai.test.xring] "
    sem.wait();

    for( std::size_t i = 0; i < loop; ++i )
    {
        //std::cout << "Loop: " << i << std::endl;
        std::vector<u_char*> v;
        v.reserve(1000);

        auto[caplen, pack_data] = reader.read();
        v.push_back(pack_data);

        while(true)
        {
            auto[caplen, pack_data] = reader.read();
            
            if(caplen == -2)
                break;

            v.push_back(pack_data);
        }
        std::cout << v.size() << std::endl;
        std::cin.get();

        while( true )
        {   
            auto[caplen, pack_data] = reader.read();
            
            if( caplen == -2 )
                break;

            if( caplen == 0 || pack_data == nullptr )
                continue;

            //LOG_CON(LOG_TAG "Produce: %d\n", ix);

            auto instance = rng.alloc<test_diss_mock>(caplen);

            instance->seq = ix;
            instance->caplen = caplen;
            instance->value = pack_data;
                
            auto slot = xslot_t::slot(instance);

            slot->commit();
            ix++;

            sem.wait();
        }

        reader.open(dump);
    }

    auto instance = rng.alloc<test_diss_mock>(1);
    instance->seq = 1;
    instance->caplen = 4;
    instance->value = (u_char*)"....";
            
    auto slot = xslot_t::slot(instance);

    slot->commit();

    LOG_CON(LOG_TAG "Producer Out: %d\n", ix);
    #undef LOG_TAG
}
    
}
