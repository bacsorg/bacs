#pragma once

#include <bunsan/pm/entry.hpp>

#include <set>
#include <map>

#include <unordered_set>

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
            ar & BOOST_SERIALIZATION_NVP(source);
            ar & BOOST_SERIALIZATION_NVP(package);
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

        // TODO make it unordered
        std::set<entry> all() const;

        std::unordered_set<std::string> sources() const;

        struct stage
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

            bool empty() const;

            std::set<entry> all() const;
        } source, package;
    };
}}
