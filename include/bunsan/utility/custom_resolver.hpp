#pragma once

#include <bunsan/utility/resolver.hpp>

#include <boost/optional.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/serialization/access.hpp>
#include <boost/serialization/nvp.hpp>
#include <boost/unordered_map.hpp>
#include <boost/unordered_set.hpp>

namespace bunsan{namespace utility
{
    class custom_resolver: public resolver
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

    public:
        /// Same as system_resolver
        custom_resolver();

        custom_resolver(std::unique_ptr<resolver> back_resolver);

        /// Object will use config-specified algorithms for path resolving.
        explicit custom_resolver(const config &config_);

        custom_resolver(const config &config_,
                        std::unique_ptr<resolver> back_resolver);

        /// resolver(config(ptree))
        explicit custom_resolver(const boost::property_tree::ptree &ptree);

        custom_resolver(const boost::property_tree::ptree &ptree,
                        std::unique_ptr<resolver> back_resolver);

        boost::filesystem::path find_executable(
            const boost::filesystem::path &exe) override;

        boost::filesystem::path find_library(
            const boost::filesystem::path &lib) override;

        std::unique_ptr<resolver> clone() const override;

    private:
        void apply_alias(boost::filesystem::path &name) const;
        void apply_absolute(boost::filesystem::path &name) const;
        void apply_path(boost::filesystem::path &name) const;
        void apply_back(boost::filesystem::path &name) const;

    private:
        boost::optional<config> m_config;
        std::unique_ptr<resolver> m_back_resolver;
    };
}}
