#pragma once

#include <sstream>
#include <vector>
#include <string>

namespace stg::mdp::mdd::serialize
{

class serializer
{
public:
    serializer() {}
    ~serializer() {}

    //TODO(Gennady Shibaev): Decide functor or function better to use
    std::ostream& operator()(std::ostream& os, const std::vector<record>& pod )
    {
        const char null = '\0';

        for( std::size_t i = 0; i < pod.size(); ++i )
        {
            auto size = pod[i].key.size();
            os.write( reinterpret_cast<const char*>(&size), uint_size );
                os.write( pod[i].key.c_str(), pod[i].key.size() );

            size = pod[i].field.size();
            os.write( reinterpret_cast<const char*>(&size), uint_size );
                os.write( pod[i].field.c_str(), pod[i].field.size() );

            size = pod[i].value.size();
            os.write( reinterpret_cast<const char*>(&size), uint_size );
                os.write( pod[i].value.c_str(), pod[i].value.size() );
            
            // end of line
            os.write( &null, 1 );
        }

        return os;
    }

    std::ostream& serializing(std::ostream& os, const std::vector<record>& pod)
    {
        char null = '\0';

        for( std::size_t i = 0; i < pod.size(); ++i )
        {//std::to_string(size).c_str(), std::to_string(size).size()
            std::size_t size = pod[i].key.size();  
            os.write( reinterpret_cast<const char*>(&size), uint_size );
                os.write( pod[i].key.c_str(), pod[i].key.size() );

            size = pod[i].field.size();
            os.write( reinterpret_cast<const char*>(&size), uint_size );
                os.write( pod[i].field.c_str(), pod[i].field.size() );

            size = pod[i].value.size();
            os.write( reinterpret_cast<const char*>(&size), uint_size );
                os.write( pod[i].value.c_str(), pod[i].value.size() );
            
            // end of line
            os.write( &null, 1 );
        }

        return os;
    }

private:
    constexpr static size_t uint_size = sizeof( std::size_t );

};

} // namespace stg::mdp::mdd::serialize