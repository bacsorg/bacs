#pragma once

#include <string>

#include <boost/property_tree/ptree.hpp>
#include <boost/serialization/access.hpp>
#include <boost/serialization/nvp.hpp>

namespace bunsan{namespace utility
{
    struct factory_options
    {
        template <typename Archive>
        void serialize(Archive &ar, const unsigned int)
        {
            ar & BOOST_SERIALIZATION_NVP(type);
            ar & BOOST_SERIALIZATION_NVP(config);
        }

        /// Try to create instance of type using Factory, setup it on success.
        template <typename Factory, typename ... Args>
        typename Factory::bunsan_factory::result_type instance(Args &&...args) const
        {
            const typename Factory::bunsan_factory::result_type tmp = Factory::instance(type, std::forward<Args>(args)...);
            if (tmp)
                tmp->setup(config);
            return tmp;
        }

        std::string type;
        boost::property_tree::ptree config;
    };
}}
