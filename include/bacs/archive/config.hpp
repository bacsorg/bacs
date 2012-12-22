#pragma once

#include "bunsan/utility/factory_options.hpp"
#include "bunsan/utility/archiver.hpp"

#include <boost/filesystem/path.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/serialization/access.hpp>
#include <boost/serialization/nvp.hpp>

namespace bacs{namespace archive
{
    struct archiver_options: bunsan::utility::factory_options<bunsan::utility::archiver>
    {
        bunsan::utility::archiver_ptr instance(const bunsan::utility::resolver &resolver) const;
    };

    struct config
    {
        template <typename Archive>
        void serialize(Archive &ar, const unsigned int)
        {
            ar & BOOST_SERIALIZATION_NVP(lock);
            ar & BOOST_SERIALIZATION_NVP(resolver);
            ar & BOOST_SERIALIZATION_NVP(tmpdir);
            ar & BOOST_SERIALIZATION_NVP(problem.archiver);
        }

        std::string lock;
        boost::property_tree::ptree resolver;
        boost::filesystem::path tmpdir;
        struct
        {
            archiver_options archiver;
        } problem;
    };
}}
