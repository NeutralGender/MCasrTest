#pragma once

#include <stg/mdp/mdd/db/db_driver.hpp>
#include <stg/mdp/mdd/db/pgsql/psql_connection.hpp>
#include <stg/mdp/mdd/db/pgsql/psql_commands.hpp>

#include <string>

namespace stg::mdp::mdd::db::pgsql
{

using stg::mdp::mdd::db::db_driver_t;

class psql_driver: 
public db_driver_t
{
public:
    psql_driver(const std::string& dbname_,
                const std::string& user_,
                const std::string& password_,
                const std::string& hostaddr_,
                const std::string& port_):
                pconnection(dbname_,
                            user_, 
                            password_, 
                            hostaddr_, 
                            port_) 
    {}

    virtual ~psql_driver() 
    {}

    void connect_db() override
    { pconnection.connect_psql(); }

    void create_table()
    { pcommands.create_table(pconnection); }

    void drop_table()
    { pcommands.drop_table(pconnection); }

    void write_log(const std::string& client_name, 
                   const std::string& status,
                   const std::string& client_ip) 
    { pcommands.write_log(pconnection,client_name,status,client_ip); }

private:
    psql_connection pconnection;
    psql_commands pcommands;

};

} // namespace stg::mdp::mdd::db::pgsql