#pragma once

#include <string>

#include <boost/property_tree/ptree.hpp>
#include <boost/serialization/access.hpp>
#include <boost/serialization/nvp.hpp>

namespace bunsan{namespace utility
{
    template <typename Factory>
    struct factory_options
    {
        typedef Factory factory_type;
        typedef typename factory_type::bunsan_factory::result_type result_type;
        typedef typename factory_type::bunsan_factory::arguments_size arguments_size;

        template <typename Archive>
        void serialize(Archive &ar, const unsigned int)
        {
            ar & BOOST_SERIALIZATION_NVP(type);
            ar & BOOST_SERIALIZATION_NVP(config);
        }

        /// Try to create instance of type using Factory, setup it on success.
        template <typename ... Args>
        typename std::enable_if<arguments_size::value == sizeof...(Args),
            result_type>::type instance(Args &&...args) const
        {
            const result_type tmp = factory_type::instance(type, std::forward<Args>(args)...);
            if (tmp)
                tmp->setup(config);
            return tmp;
        }

        std::string type;
        boost::property_tree::ptree config;
    };
}}
