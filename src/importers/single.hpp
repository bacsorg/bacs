#pragma once

#include "bacs/problem/importer.hpp"
#include "bacs/problem/error.hpp"

#include "bacs/single/problem/generator.hpp"

#include <boost/serialization/access.hpp>
#include <boost/serialization/nvp.hpp>

namespace bacs{namespace problem{namespace importers
{
    struct problem_info_serialization_error: virtual error {};

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
        const bacs::single::problem::generator_ptr m_generator;

    private:
        static const bool factory_reg_hook;
    };
}}}
