#pragma once

#include <fstream>
#include <exception>

namespace stg::mdp::mdd::common::writer
{

class writer_t
{
public:
    writer_t(const std::string& filename) try : filename(filename)
    { 
        out.open(filename,std::ios_base::trunc); 
        if( ! out.is_open() )
            throw std::exception();
    }
    catch( ... )
    { std::cout << "Exception()\n"; }

    ~writer_t() { out.close(); }

    static void write(const std::string data)
    { out << data; }

protected:

private:
    const std::string filename;
    static std::ofstream out;
};

std::ofstream writer_t::out;

}