#pragma once

#include <pqxx/pqxx>

namespace stg::mdp::mdd::db::pgsql
{

class psql_commands
{    
public:
    psql_commands() 
    {}

    ~psql_commands() 
    {}

    void create_table(psql_connection& p_conn)
    {
        try
        {
            std::string sql = "CREATE TABLE IF NOT EXISTS MulticastLogs\
                                            (\
                                                Object varchar(50),\
                                                TimeStamp TIMESTAMP,\
                                                Status varchar(50),\
                                                ClientIp varchar(50)\
                                            );";
            pqxx::work W( p_conn.connect );
            pqxx::result R = W.exec(sql); 
            W.commit();
        }
        catch(const std::exception& e)
        {
            std::cerr << e.what() << errno << '\n';
        }
    }

    void drop_table(psql_connection& p_conn)
    {
        try
        {
            std::string sql = "DROP TABLE IF EXISTS MulticastLogs";
            pqxx::work W( p_conn.connect );
            pqxx::result R = W.exec(sql); 
            W.commit();
        }
        catch(const std::exception& e)
        {
            std::cerr << e.what() << errno << '\n';
        }
    }

    void write_log(psql_connection& p_conn,
                   const std::string& client_name, 
                   const std::string& status,
                   const std::string& client_ip)
    {
        try
        {
            p_conn.connect.prepare( "INSERT_TABLE", 
                             "INSERT INTO MulticastLogs VALUES( $1, now(), $2, $3 )" );

            pqxx::work W( p_conn.connect );
            pqxx::result R = W.exec_prepared("INSERT_TABLE", client_name, status, client_ip );

            W.commit();
        }
        catch(const std::exception& e)
        {
            std::cerr << e.what() << '\n';
        }
    }
};

} // namespace stg::mdp::mdd::db::pgsql
