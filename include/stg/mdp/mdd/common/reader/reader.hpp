#pragma once

#include <string>
#include <vector>
#include <memory>
#include <map>

namespace stg::mdp::mdd::common::reader
{

/**
 * @brief High-level module for reading files
 */
template <typename T>
class reader_t
{
public:
    explicit reader_t(T& api_reader):
                      m_reader{api_reader}
    {}

    ~reader_t()
    {}

    bool read_config(const std::string& filename)
    {
        if( not m_reader.open(filename) )
            return false;
        
        read();

        return content();
    }

    /**
     * @brief Open file
     * 
     * @param filename filename captured package filename
     * @return true if file was opened
     * @return false if cannot open file
     */
    [[nodiscard]]
    bool open( const std::string& filename )
    { return ( not m_reader.open(filename) ) ? false : true; }

    /**
     * @brief Read file
     * 
     * @return const auto 
     */
    const auto read()
    { return ( m_reader.read() ); }

    /**
     * @brief Get the data object
     * 
     * @return const auto& 
     */
    const auto& content()
    { return (m_reader.get_data()); }

protected:

private:
    T& m_reader;
};

}
