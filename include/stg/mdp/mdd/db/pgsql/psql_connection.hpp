#pragma once

#include <pqxx/pqxx>

namespace stg::mdp::mdd::db::pgsql
{

class psql_connection
{
public:
    psql_connection(const std::string& dbname_,
                    const std::string& user_,
                    const std::string& password_,
                    const std::string& hostaddr_,
                    const std::string& port_):
                    connect("dbname=" + dbname_
                         + " user=" + user_
                         + " password=" + password_
                         + " hostaddr=" + hostaddr_
                         + " port=" + port_) 
    {}

    ~psql_connection() 
    {}

    void connect_psql()
    {
        try
        {
            if (!connect.is_open())
                throw std::runtime_error("Cannot open DB\n");
        }
        catch (const std::runtime_error &r)
        {
            std::cout << r.what() << std::endl;
        }
    }

    friend class psql_commands;

private:
    pqxx::connection connect;

};

} // namespace stg::mdp::mdd::db::pgsql