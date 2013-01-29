#pragma once

#include "bunsan/utility/factory_options.hpp"
#include "bunsan/utility/archiver.hpp"
#include "bunsan/utility/builder.hpp"
#include "bunsan/utility/fetcher.hpp"

#include <string>

#include <boost/optional.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/filesystem/path.hpp>
#include <boost/serialization/access.hpp>
#include <boost/serialization/nvp.hpp>

namespace bunsan{namespace pm
{
    namespace detail
    {
        struct factory_config
        {
            template <typename Archive>
            void serialize(Archive &ar, const unsigned int)
            {
                ar & BOOST_SERIALIZATION_NVP(type);
                ar & BOOST_SERIALIZATION_NVP(config);
            }

            std::string type;
            boost::property_tree::ptree config;
        };
    }

    struct config
    {
        template <typename Archive>
        void serialize(Archive &ar, const unsigned int)
        {
            ar & BOOST_SERIALIZATION_NVP(lock.global);
            ar & BOOST_SERIALIZATION_NVP(dir);
            ar & BOOST_SERIALIZATION_NVP(name);
            ar & BOOST_SERIALIZATION_NVP(utility);
            ar & BOOST_SERIALIZATION_NVP(repository_url);
        }

        struct
        {
            boost::optional<boost::filesystem::path> global;    ///< global lock for synchronization
        } lock;

        struct
        {
            template <typename Archive>
            void serialize(Archive &ar, const unsigned int)
            {
                ar & BOOST_SERIALIZATION_NVP(tmp);
                ar & BOOST_SERIALIZATION_NVP(package);
                ar & BOOST_SERIALIZATION_NVP(source);
            }

            boost::filesystem::path tmp;        ///< directory for large temporary files
            boost::filesystem::path package;    ///< directory for built packages
            boost::filesystem::path source;     ///< directory for package sources
        } dir;

        struct
        {
            template <typename Archive>
            void serialize(Archive &ar, const unsigned int)
            {
                ar & BOOST_SERIALIZATION_NVP(file);
                ar & BOOST_SERIALIZATION_NVP(dir);
                ar & BOOST_SERIALIZATION_NVP(suffix);
            }

            struct
            {
                template <typename Archive>
                void serialize(Archive &ar, const unsigned int)
                {
                    ar & BOOST_SERIALIZATION_NVP(tmp);
                    ar & BOOST_SERIALIZATION_NVP(index);
                    ar & BOOST_SERIALIZATION_NVP(checksum);
                    ar & BOOST_SERIALIZATION_NVP(build_snapshot);
                    ar & BOOST_SERIALIZATION_NVP(installation_snapshot);
                    ar & BOOST_SERIALIZATION_NVP(build);
                    ar & BOOST_SERIALIZATION_NVP(installation);
                }

                std::string tmp;                    ///< file mask for small temporary files,
                                                    ///< should be absolute
                std::string index;                  ///< index file name
                std::string checksum;               ///< checksum file name
                std::string build_snapshot;         ///< build snapshot file name
                std::string installation_snapshot;  ///< installation snapshot file name
                std::string build;                  ///< build archive name
                std::string installation;           ///< installation archive name
            } file;

            struct
            {
                template <typename Archive>
                void serialize(Archive &ar, const unsigned int)
                {
                    ar & BOOST_SERIALIZATION_NVP(build);
                    ar & BOOST_SERIALIZATION_NVP(installation);
                }

                boost::filesystem::path source;         ///< subdirectory for package source
                boost::filesystem::path build;          ///< subdirectory for package building
                boost::filesystem::path installation;   ///< subdirectory for package installation
            } dir;

            struct
            {
                template <typename Archive>
                void serialize(Archive &ar, const unsigned int)
                {
                    ar & BOOST_SERIALIZATION_NVP(source_archive);
                }

                std::string source_archive; ///< suffix of archive files
            } suffix;
        } name;

        struct
        {
            template <typename Archive>
            void serialize(Archive &ar, const unsigned int)
            {
                ar & BOOST_SERIALIZATION_NVP(resolver);
                ar & BOOST_SERIALIZATION_NVP(builder);
                ar & BOOST_SERIALIZATION_NVP(fetcher);
                ar & BOOST_SERIALIZATION_NVP(source_archiver);
                ar & BOOST_SERIALIZATION_NVP(cache_archiver);
            }

            boost::property_tree::ptree resolver;
            utility::factory_options<utility::builder> builder;
            utility::factory_options<utility::fetcher> fetcher;
            utility::factory_options<utility::archiver> source_archiver, cache_archiver;
        } utility;

        std::string repository_url; ///< repository location
    };
}}
