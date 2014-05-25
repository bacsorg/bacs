#include <bacs/problem/utility.hpp>

#include <bacs/problem/error.hpp>

#include <boost/filesystem/operations.hpp>
#include <boost/property_tree/ini_parser.hpp>

namespace bacs{namespace problem
{
    BUNSAN_FACTORY_DEFINE(utility)

    namespace
    {
        std::string get_builder(
            const boost::property_tree::ptree &config)
        {
            // note: "no" is equivalent to empty section
            return config.get<std::string>("build.builder", "none");
        }

        boost::property_tree::ptree get_config(
            const boost::filesystem::path &location)
        {
            boost::property_tree::ptree config;
            boost::property_tree::read_ini(
                (location / "config.ini").string(),
                config
            );
            return config;
        }
    }

    utility_ptr utility::instance(
        const boost::filesystem::path &location)
    {
        return instance(location, get_config(location));
    }

    utility_ptr utility::instance_optional(
        const boost::filesystem::path &location)
    {
        return instance_optional(location, get_config(location));
    }

    utility_ptr utility::instance(
        const boost::filesystem::path &location,
        const boost::property_tree::ptree &config)
    {
        return instance(get_builder(config), location, config);
    }

    utility_ptr utility::instance_optional(
        const boost::filesystem::path &location,
        const boost::property_tree::ptree &config)
    {
        return instance_optional(get_builder(config), location, config);
    }

    utility::utility(
        const boost::filesystem::path &location,
        const boost::property_tree::ptree &config):
            m_location(location),
            m_target(config.get<std::string>("build.target",
                boost::filesystem::absolute(location).filename().string())),
            m_config(config) {}

    Utility utility::info() const
    {
        Utility info_;
        info_.set_builder(get_builder(m_config));
        return info_;
    }

    boost::property_tree::ptree utility::section(
        const std::string &name) const
    {
        return m_config.get_child(name);
    }

    boost::filesystem::path utility::location() const
    {
        return m_location;
    }

    boost::filesystem::path utility::target() const
    {
        if (m_target.filename() != m_target)
            BOOST_THROW_EXCEPTION(
                invalid_target_error() <<
                invalid_target_error::target(m_target));
        return m_target;
    }
}}
