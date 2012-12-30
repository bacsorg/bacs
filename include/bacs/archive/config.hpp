#pragma once

#include "bacs/archive/archiver_options.hpp"

#include <boost/filesystem/path.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/serialization/access.hpp>
#include <boost/serialization/nvp.hpp>

namespace bacs{namespace archive
{
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

        boost::filesystem::path lock;
        boost::property_tree::ptree resolver;
        boost::filesystem::path tmpdir;
        struct
        {
            archiver_options archiver;
        } problem;
    };
}}
