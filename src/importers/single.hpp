#pragma once

#include "bacs/problem/importer.hpp"

#include <boost/serialization/access.hpp>
#include <boost/serialization/nvp.hpp>

namespace bacs{namespace problem{namespace importers
{
    class single: public importer::impl
    {
    public:
        struct config
        {
            template <typename Archive>
            void serialize(Archive &ar, const unsigned int)
            {
                ar & BOOST_SERIALIZATION_NVP(generator.type);
                ar & BOOST_SERIALIZATION_NVP(generator.config);
            }

            struct
            {
                std::string type;
                boost::property_tree::ptree config;
            } generator;
        };

    public:
        explicit single(const boost::property_tree::ptree &config_);

        info convert(const options &options_) override;

    private:
        const config m_config;

    private:
        static const bool factory_reg_hook;
    };
}}}
