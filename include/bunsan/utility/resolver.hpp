#pragma once

#include <bunsan/utility/error.hpp>

#include <boost/filesystem/path.hpp>
#include <boost/optional.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/serialization/access.hpp>
#include <boost/serialization/nvp.hpp>
#include <boost/unordered_map.hpp>
#include <boost/unordered_set.hpp>

namespace bunsan{namespace utility
{
    struct resolver_error: virtual error {};
    struct resolver_find_error: virtual resolver_error {};
    struct resolver_find_executable_error: virtual resolver_find_error
    {
        typedef boost::error_info<
            struct tag_executable,
            boost::filesystem::path
        > executable;
    };
    struct resolver_find_library_error: virtual resolver_find_error
    {
        typedef boost::error_info<
            struct tag_library,
            boost::filesystem::path
        > library;
    };
    struct resolver_circular_alias_dependencies_error:
        virtual resolver_error {};

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

            boost::unordered_map<boost::filesystem::path,
                                 boost::filesystem::path> alias, absolute;
            boost::unordered_set<boost::filesystem::path> path;
        };

        static void swap(config &a, config &b) noexcept
        {
            using std::swap;
            swap(a.alias, b.alias);
            swap(a.absolute, b.absolute);
            swap(a.path, b.path);
        }

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

        void swap(resolver &r) noexcept
        {
            using std::swap;
            swap(m_config, r.m_config);
        }

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
}}
