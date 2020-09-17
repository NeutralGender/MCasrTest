cmake_minimum_required(VERSION 3.16.3)
project(multicast)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17 -Wall" )

find_library( CryptoPP_LIB cryptopp )
find_library( Hiredis_LIB hiredis )
find_library( PQ_LIB pq )
find_library( PQXX_LIB pqxx )
find_library( BOOST_SYSTEM_LIB boost_system )
find_library( BOOST_SERIALIZATION_LIB boost_serialization )

set( MAIN "${PROJECT_SOURCE_DIR}/src/main.cpp" )

include_directories("${PROJECT_SOURCE_DIR}/include")
include_directories("${PROJECT_SOURCE_DIR}/include/stg/mdp/mdd")

add_executable( ${PROJECT_NAME} ${MAIN} )

target_link_libraries( ${PROJECT_NAME} 
                       ${CryptoPP_LIB}
                       ${Hiredis_LIB}
                       ${PQ_LIB}
                       ${PQXX_LIB}
                       ${BOOST_SYSTEM_LIB}
                       ${BOOST_SERIALIZATION_LIB} )

