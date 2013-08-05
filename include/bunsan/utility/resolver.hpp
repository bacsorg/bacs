#pragma once

#include <bunsan/utility/error.hpp>

#include <boost/optional.hpp>
#include <boost/unordered_map.hpp>
#include <boost/unordered_set.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/filesystem/path.hpp>
#include <boost/serialization/access.hpp>
#include <boost/serialization/nvp.hpp>

namespace bunsan{namespace utility
{
    class resolver
    {
    public:
        struct config
        {
            template <typename Archive>
            void serialize(Archive &ar, const unsigned int)
            {
                ar & BOOST_SERIALIZATION_NVP(alias);
                ar & BOOST_SERIALIZATION_NVP(absolute);
                ar & BOOST_SERIALIZATION_NVP(path);
            }

            boost::unordered_map<boost::filesystem::path, boost::filesystem::path> alias, absolute;
            boost::unordered_set<boost::filesystem::path> path;
        };

        static void swap(config &a, config &b) noexcept;

    public:
        /// Object will use os-specified algorithms for path resolving.
        resolver();

        /// Object will use config-specified algorithms for path resolving.
        explicit resolver(const config &config_);

        /// \copy_doc resolver::resolver()
        explicit resolver(const boost::property_tree::ptree &ptree);

        resolver(const resolver &)=default;
        resolver(resolver &&) noexcept;
        resolver &operator=(const resolver &);
        resolver &operator=(resolver &&) noexcept;

        void swap(resolver &) noexcept;

        // interface
        boost::filesystem::path find_executable(
            const boost::filesystem::path &exe) const;

        boost::filesystem::path find_library(
            const boost::filesystem::path &lib) const;

    private:
        void apply_alias(boost::filesystem::path &name) const;
        void apply_absolute(boost::filesystem::path &name) const;
        void apply_path(boost::filesystem::path &name) const;

    private:
        boost::optional<config> m_config;
    };
    inline void swap(resolver &a, resolver &b) noexcept
    {
        a.swap(b);
    }

    struct resolver_error: virtual error {};
    struct resolver_circular_alias_dependencies_error: virtual resolver_error {};
}}
