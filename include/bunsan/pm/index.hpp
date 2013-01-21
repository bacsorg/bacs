#pragma once

#include "bunsan/pm/entry.hpp"

#include <set>
#include <map>

#include <boost/filesystem/path.hpp>
#include <boost/serialization/access.hpp>
#include <boost/serialization/nvp.hpp>

namespace bunsan{namespace pm
{
    struct index
    {
        template <typename Archive>
        void serialize(Archive &ar, const unsigned int)
        {
            ar & BOOST_SERIALIZATION_NVP(package);
            ar & BOOST_SERIALIZATION_NVP(source);
        }

        index()=default;
        index(const index &)=default;
        index(index &&)=default;
        index &operator=(const index &)=default;
        index &operator=(index &&)=default;

        template <typename T>
        explicit index(const T &obj)
        {
            load(obj);
        }

        explicit operator boost::property_tree::ptree() const;

        void load(const boost::property_tree::ptree &ptree);
        void load(const boost::filesystem::path &path);
        void save(const boost::filesystem::path &path) const;

        std::set<entry> all() const;

        std::multimap<boost::filesystem::path, entry> package;
        struct
        {
            template <typename Archive>
            void serialize(Archive &ar, const unsigned int)
            {
                ar & BOOST_SERIALIZATION_NVP(self);
                ar & BOOST_SERIALIZATION_NVP(import.source);
                ar & BOOST_SERIALIZATION_NVP(import.package);
            }

            struct
            {
                std::multimap<boost::filesystem::path, entry> source, package;
            } import;
            std::multimap<boost::filesystem::path, std::string> self;
        } source;
    };
}}
