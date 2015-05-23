#include <bunsan/utility/resolver.hpp>

#include <bunsan/utility/error.hpp>

#include <bunsan/config/cast.hpp>
#include <bunsan/process/path.hpp>

namespace bunsan{namespace utility
{
    resolver::resolver() {}

    resolver::resolver(const config &config_): m_config(config_) {}

    resolver::resolver(const boost::property_tree::ptree &ptree):
        resolver(bunsan::config::load<config>(ptree)) {}

    resolver::resolver(resolver &&res) noexcept
    {
        swap(res);
    }

    bunsan::utility::resolver &resolver::operator=(const resolver &res)
    {
        resolver tmp(res);
        swap(tmp);
        return *this;
    }

    bunsan::utility::resolver &resolver::operator=(resolver &&res) noexcept
    {
        swap(res);
        return *this;
    }

    void resolver::apply_alias(boost::filesystem::path &name) const
    {
        if (!m_config)
            return;
        boost::unordered_set<boost::filesystem::path> used;
        used.insert(name);
        auto iter = m_config->alias.find(name);
        // try to resolve as alias while possible
        while (iter != m_config->alias.end())
        {
            name = iter->second;
            iter = m_config->alias.find(name);
            if (used.find(name) != used.end())
                BOOST_THROW_EXCEPTION(resolver_circular_alias_dependencies_error());
            used.insert(name);
        }
    }

    void resolver::apply_absolute(boost::filesystem::path &name) const
    {
        if (!m_config)
            return;
        const auto iter = m_config->absolute.find(name);
        if (iter != m_config->absolute.end())
            name = iter->second;
    }

    void resolver::apply_path(boost::filesystem::path &name) const
    {
        if (m_config)
        {
            if (m_config->path.find(name) != m_config->path.end() && name == name.filename())
                name = bunsan::process::find_executable_in_path(name);
        }
        else
        {
            if (name == name.filename())
                name = bunsan::process::find_executable_in_path(name);
        }
    }

    boost::filesystem::path resolver::find_executable(const boost::filesystem::path &exe) const
    {
        try
        {
            boost::filesystem::path ret = exe;
            apply_alias(ret);
            apply_absolute(ret);
            apply_path(ret);
            return ret;
        }
        catch (std::exception &)
        {
            BOOST_THROW_EXCEPTION(resolver_find_executable_error() <<
                                  resolver_find_executable_error::executable(exe) <<
                                  enable_nested_current());
        }
    }

    boost::filesystem::path resolver::find_library(const boost::filesystem::path &lib) const
    {
        try
        {
#warning unimplemented
            return lib;
        }
        catch (std::exception &)
        {
            BOOST_THROW_EXCEPTION(resolver_find_library_error() <<
                                  resolver_find_library_error::library(lib) <<
                                  enable_nested_current());
        }
    }
}}
