#include "bacs/problem/utility.hpp"
#include "bacs/problem/error.hpp"

#include <boost/property_tree/ini_parser.hpp>
#include <boost/filesystem/operations.hpp>

namespace bacs{namespace problem
{
    BUNSAN_FACTORY_DEFINE(utility)

    utility_ptr utility::instance(const boost::filesystem::path &location)
    {
        boost::property_tree::ptree config;
        boost::property_tree::read_ini((location / "config.ini").string(), config);
        // note: "no" is equivalent to empty section
        return instance(config.get<std::string>("build.builder", "no"), location, config);
    }

    utility_ptr utility::instance_optional(const boost::filesystem::path &location)
    {
        try
        {
            return instance(location);
        }
        catch (unknown_utility_error &)
        {
            return utility_ptr();
        }
    }

    utility::utility(const boost::filesystem::path &location,
                     const boost::property_tree::ptree &config):
        m_location(location),
        m_target(config.get<std::string>("build.target",
            boost::filesystem::absolute(location).filename().string())),
        m_config(config) {}

    pb::Utility utility::info() const
    {
        pb::Utility info_;
        info_.set_builder(m_config.get<std::string>("build.builder"));
        return info_;
    }

    boost::property_tree::ptree utility::section(const std::string &name) const
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
            BOOST_THROW_EXCEPTION(invalid_target_error() <<
                                  invalid_target_error::target(m_target));
        return m_target;
    }
}}
