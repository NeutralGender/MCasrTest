#include <iostream>
#include <filesystem>

namespace fs = std::filesystem;

#include <stg/mdp/mdd/conf/mcast/mcast_conf.hpp>
using stg::mdp::mdd::mcast::conf::mcast_conf;

#include <stg/mdp/mdd/conf/dissemination/production.hpp>
using stg::mdp::mdd::conf::dissemination::diss_prod_conf;

// xring
#include <stg/ai/xring/xring.hpp>
using stg::ai::xring::xring_t;
using stg::ai::xring::xring_conf;
using stg::ai::xring::xslot_t;
using stg::ai::xring::xring_fetch_t;
using stg::ai::xring::xring_err;

// additional 
#include <stg/mdp/mdd/net/multicast/types.hpp>

using stg::mdp::mdd::net::multicast::mcast_type;


#include <stg/mdp/mdd/dissemination/server/producer/producer.hpp>
using stg::mdp::mdd::server::prod::producer;

//#include <stg/mdp/mdd/dissemination/server/consm/consumer.hpp>
//using stg::mdp::mdd::dissemination::server::consm::consumer;

#include <stg/mdp/mdd/dissemination/server/dissamination/dissamination.hpp>
using stg::mdp::mdd::dissm_server::dissm_sender;

//#include <stg/mdp/mdd/dissemination/client/dissemination/dissemination.hpp>
//using stg::mdp::mdd::dissemination::client::dissemination::dissm_receiver;

////#include <stg/mdp/mdd/dissemination/client/client.hpp>
//using stg::mdp::mdd::client::client_t;

#include <stg/mdp/mdd/dissemination/server/server.hpp>
using stg::mdp::mdd::dissemination::server::server_t;

#include <stg/mdp/mdd/common/reader/reader.hpp>
using stg::mdp::mdd::common::reader::reader_t;

#include <stg/mdp/mdd/common/reader/pcap/pcap_reader.hpp>
using stg::mdp::mdd::common::reader::pcap::pcap_reader;

#include <stg/mdp/mdd/common/reader/filesys/json_reader.hpp>
using stg::mdp::mdd::common::fs::json_reader;

#include <stg/mdp/mdd/common/json_gnr/json_generator/json_grt.hpp>
using stg::mdp::mdd::common::json_gnr::gnr::json_grt;

#include <stg/mdp/mdd/common/json_gnr/writer/writer.hpp>
using stg::mdp::mdd::common::writer::writer_t;

#define LOG_TAG "[stg.ai.perf.xring] "
#include <stg/ai/log.hpp>
using stg::ai::path_t;

void poll()
{
    char a;
    std::cout << "Please, enter Y(Ok)/N(false)" << std::endl;
    std::cin >> a;
    switch(a)
    {
        case 'Y':
                { break; }
        case 'y':
                { break; }
        default: { exit(0); }
    }
}

int main()
{
    LOG_INSTANTIATE(path_t("log"), "stg.ai.perf.xring");
    LOG_CON(LOG_TAG "%s", "Running XRING perf test");

    LOG_FLUSH;
    LOG_AUDIT(LOG_TAG "%s", "Running XRING perf test" );
    LOG_FLUSH;
    LOG_CON_FORCE(LOG_TAG "%s", "Running XRING perf test");
    LOG_CON_FORCE(LOG_TAG "%s", CON_RED("Press Ctrl-Break to terminate..."));

    const fs::path workdir = fs::current_path();
    const fs::path prod_path = workdir / "etc" / "json" / "dissemination" / "prod.conf.json";
    //const char* prod_path = "etc/json/dissemination/prod.conf.json";
    
    const fs::path prod_addr = workdir / "etc" / "json" / "dissemination" / "prod_addr.json";
    //const char* prod_addr = "etc/json/dissemination/prod_addr.json";
    
    const fs::path mcsat_path = workdir / "etc" / "json" / "mcast_conf.json";
    //const char* mcsat_path = "etc/json/mcast_conf.json";
    
    const fs::path pcap_path = workdir / "capture" / "multicast.pcapng";
    //const char* pcap_path = "/capture/multicast.pcapng";
    
    const fs::path flood = workdir / "capture" / "flooding.pcapng";
    //const char* flood = "/capture/flooding.pcapng";

    reader_t<json_reader> mcast_reader( *(new json_reader ) );

    if( not mcast_reader.open(mcsat_path.c_str()) )
      return false;

    mcast_reader.read();

    mcast_conf mconf( mcast_reader.content() );

    {
        writer_t w(prod_addr.c_str());

        std::cout << mconf.mcast_sender.local << std::endl;
        json_grt gnrt(mconf.mcast_sender.local, mconf.port);

        std::string ip_addr = mconf.ip_addr_start;

        gnrt.build_config(mconf.json_addresses, ip_addr);
        gnrt.write();
    }

    reader_t<json_reader> prod_reader( *(new json_reader ) );
    if( not prod_reader.open(prod_addr.c_str()) )
    {
        std::cout << "Cannot open json" << std::endl;
        return false;
    }
    prod_reader.read();

    diss_prod_conf dprod( prod_reader.content() );

    xring_conf xconf;
    xconf.setup_capacity( 200 );

    xring_t<> rng ( xconf, "" );

    stg::ai::ai_wait_t sem;

    reader_t<pcap_reader> reader( *(new pcap_reader) );
    
    if( not reader.open(mconf.dump_file.c_str()) )
    {
        std::cout << "Cannot open pcap" << std::endl;
        return false;
    }

    if( mconf.delay_us != 0 )
    {
        auto start_time = std::chrono::system_clock::now();
        for( std::size_t i = 0; i < mconf.delay_us; ++i )
        {
            
        }
        auto end_time = std::chrono::system_clock::now();
        std::chrono::duration<double, std::micro> fp_ms = end_time - start_time;
        std::cout << "Delay test: " << fp_ms.count() << std::endl;

        poll();

        std::atomic<std::size_t> _consume_ix_ = 0;
        double time = 0;
        {
            server_t server(dprod.prod_a.channels, mconf.delay_us);
            if( not server.init_sender() )
            {
                std::cout << "Cannot server init" << std::endl;
                return false;
            }

            server.send_without_ring_micro(sem, 
                                           _consume_ix_, 
                                           mconf.dump_file, 
                                           time, 
                                           mconf.cloop, 
                                           mconf.cores,
                                           mconf.threads,
                                           mconf.package);
        }
        std::cout << "Send: " << _consume_ix_ << std::endl;
        std::cout << "Average for one thread(ms): " << static_cast<double>( (time) / ((mconf.cores * mconf.threads)) ) << std::endl;
        std::cout << "Pack/Sec: " << static_cast<double>( (_consume_ix_) / ( (time) / (mconf.cores * mconf.threads) / (1'000) ) ) << std::endl;
    }
    else
    {
        std::atomic<std::size_t> _consume_ix_ = 0;
        double time = 0;
        {
            server_t server(dprod.prod_a.channels, mconf.delay_ms*1000);
            if( not server.init_sender() )
            {
                std::cout << "Cannot server init" << std::endl;
                return false;
            }

            server.send_without_ring(sem, 
                                     _consume_ix_, 
                                     mconf.dump_file, 
                                     time, 
                                     mconf.cloop, 
                                     mconf.cores, 
                                     mconf.threads,
                                     mconf.package);
        }
        std::cout << "Send: " << _consume_ix_ << std::endl;
        std::cout << "Average for one thread(ms): " << static_cast<double>( (time) / ((mconf.cores * mconf.threads)) ) << std::endl;
        std::cout << "Pack/Sec: " << static_cast<double>( (_consume_ix_) / ( (time) / (mconf.cores * mconf.threads) / (1'000) ) ) << std::endl;
    }
    



/*
    dissm_receiver<mcast_type::ANY> rA(dprod.prod_a.channels);
    if( not rA.init(dprod.prod_a.channels, mconf ) )
        return false;

    dissm_receiver<mcast_type::ANY> rB(dprod.prod_b.channels);
    if( not rB.init(dprod.prod_b.channels, mconf ) )
        return false;
    
    client_t client(6, rng);
    client.create_epoll_fd();
    client.add_channels_epoll( rA, rB );
    client.create_thread();
*/

    return 0;
}

#undef LOG_TAG