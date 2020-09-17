#pragma once

#include <string>

/*  LEFT FOR TESTS
    #include <boost/archive/text_oarchive.hpp>
    #include <boost/archive/text_iarchive.hpp>
    #include <boost/serialization/access.hpp>
*/

namespace stg::mdp::mdd::serialize
{

struct record
{
public:
    std::string key;
    std::string field;
    std::string value;

public:
    record(const std::string& key_,
           const std::string& field_,
           const std::string& value_):
                            key(key_), 
                        field(field_), 
                        value(value_)
          { 
              key.reserve( 8 ); 
              field.reserve( 8 );
              value.reserve( 8 );
          }
    ~record() {}

    //TODO(Gennady Shibaev): Test and decide which of variant better: boost or custom
/*
    const std::string GetKey()
    { return this->key; }

    const std::string GetField()
    { return this->field; }

    const std::string GetValue()
    { return this->value; }

    template< class Archive >
    void save(Archive &ar, const unsigned int version)
    {
        ar & key;
        ar & field;
        ar & value;
    }

    template< class Archive >
    void load(Archive &ar, const unsigned int version)
    {
        std::string key;
        std::string field;
        std::string value;

        ar & key;
        ar & field;
        ar & value;
    }

    template< class Archive >
    void serialize(Archive &ar, const unsigned int version)
    {
        ar & key;
        ar & field;
        ar & value;
    }
    */

};

} // namespace stg::mdp::mdd::serialize