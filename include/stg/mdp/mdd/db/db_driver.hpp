#pragma once

#include <vector>
#include <string>

#include <stg/mdp/mdd/serialize/record.hpp>

namespace stg::mdp::mdd::db
{

using stg::mdp::mdd::serialize::record;


/**
 * @brief Abstract class for DB drivers
 * 
 */
class db_driver_t
{
public:
    virtual ~db_driver_t() 
    {}

    virtual void connect_db() = 0;

    virtual void get_all_dataR(std::vector<record>&) 
    {}

    virtual void authentication(const std::string& key) 
    {}

    /**
     * @brief SSL connection for redis db
     * 
     * @param CA_crt path to Certification Authority certificate
     * @param client_cert path to client certificate
     * @param client_private_key path client private key
     * @param server_to_request 
     */
    virtual void secure_connect(const std::string& CA_crt, 
                                const std::string& client_cert,
                                const std::string& client_private_key,
                                const std::string& server_to_request) 
    {}

    virtual std::size_t size() 
    {}

};

} // namespace stg::mdp::mdd::db