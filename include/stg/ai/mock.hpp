/**
 * \file mock.hpp
 *
 * STG test struct
 *
 * \author Gennady Shibaev
 * \copyright STG
 *
 * NOTICE:  All information contained herein is, and remains the property of mentioned above and
 * its suppliers, if any.
 *
 * The intellectual and technical concepts contained herein are proprietary to mentioned above and
 * its suppliers and may be covered by U.S. and Foreign Patents, patents in process, and are
 * protected by trade secret or copyright law.
 *
 * Dissemination of this information or reproduction of this material is strictly forbidden unless
 * prior written permission is obtained from proprietors.
 *
 */

#pragma once

#define HDR_SIZE 8

#include <cstdint>
#include <string.h>
#include <random>
#include <vector>
#include <string>
#include <stg/ai/common.hpp>
#include <stg/ai/chrono/tm_cast.hpp>
#include <stg/ai/xring/types.hpp>

namespace stg::ai {

/**
* @brief Test struct
*/
//template <std::size_t T>
struct mock
{

    mock() : value(5, 1)
    {}


    std::size_t latency() const
    {
        return (exit.tv_sec * tm_cast::sec2ns + exit.tv_nsec) -
                (entry.tv_sec * tm_cast::sec2ns + entry.tv_nsec);
    }

    stg::ai::xring::sequence_type seq;

    timespec entry{ 0, 0 };
    timespec exit{ 0, 0 };

    /**
    * @brief array for ring content testing
    */
    //static std::vector < u_char > value;
    std::string value;
    //std::array< u_char, T > value;

};
//template <std::size_t T>
//std::string mock::value;
//std::vector< u_char > mock::value = std::vector < u_char >(3);

constexpr std::size_t capacity( const std::size_t& chunks, const std::size_t& offset )
{ return ( cacheline_sz * chunks - offset - HDR_SIZE ); }


/**
* @brief Class for testing range, in which ring writes data 
*/
class memory_snapshot
{
    public:
        explicit memory_snapshot( int snap_size ): snap( snap_size, 0 ) {}
        ~memory_snapshot(){}
            
        /**
        * @brief MemCopy - copy mem. block of ring before allocation any slot
        * @param whole ring memory block
        */
        void mem_copy( u_char *const memblk )
        { 
            memcpy( (void*)&snap[0], memblk, snap.size() ); 
            std::cout << "cpy: " << memcmp( &snap[ 0 ], memblk, snap.size() ) << std::endl;
        }

        /**
        * @brief MemCompare - compare NOT written by ring memory block
        * @param chk_mem placement for new chunk, calculate  independently
        * @param seq2mem placement for new chunk, returns by memseq2mem
        * @return true if memblock started from chk_mem equal memblk started from seq2mem
        */
        bool mem_compare( std::size_t chk_mem, u_char * seq2mem )
        { 
            return ( memcmp( &snap[ chk_mem ], seq2mem, snap.size() - ( chk_mem ) ) == 0 
                            ? true : false );
        }

    private:
        std::vector< u_char > snap;

};

u_char random_number()
{
    return ( std::rand() % 255 );
}

} // anonym namespace