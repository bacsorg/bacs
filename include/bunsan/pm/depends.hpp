#pragma once

#include "bunsan/pm/entry.hpp"

#include <map>

#include <boost/filesystem/path.hpp>

namespace bunsan{namespace pm
{
    struct depends
    {
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
