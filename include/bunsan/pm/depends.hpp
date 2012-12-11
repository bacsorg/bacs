#pragma once

#include "bunsan/pm/entry.hpp"

#include <map>

#include <boost/filesystem/path.hpp>
#include <boost/serialization/access.hpp>
#include <boost/serialization/nvp.hpp>

namespace bunsan{namespace pm
{
    struct depends
    {
        template <typename Archive>
        void serialize(Archive &ar, const unsigned int)
        {
            ar & BOOST_SERIALIZATION_NVP(package);
            ar & BOOST_SERIALIZATION_NVP(source.self);
            ar & BOOST_SERIALIZATION_NVP(source.import.source);
            ar & BOOST_SERIALIZATION_NVP(source.import.package);
        }

        depends()=default;
        depends(const depends &)=default;
        depends(depends &&)=default;
        depends &operator=(const depends &)=default;
        depends &operator=(depends &&)=default;

        explicit operator boost::property_tree::ptree() const;

        template <typename T>
        explicit depends(const T &obj)
        {
            load(obj);
        }

        void load(const boost::property_tree::ptree &index);
        void load(const boost::filesystem::path &path);
        void save(const boost::filesystem::path &path) const;

        std::vector<entry> all() const;

        std::multimap<boost::filesystem::path, entry> package;
        struct
        {
            struct
            {
                std::multimap<boost::filesystem::path, entry> source, package;
            } import;
            std::multimap<boost::filesystem::path, std::string> self;
        } source;
    };
}}
