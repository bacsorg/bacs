#include <bunsan/utility/resolver.hpp>

#include <bunsan/utility/error.hpp>

#include <bunsan/config/cast.hpp>

#include <boost/process.hpp>

bunsan::utility::resolver::resolver() {}

bunsan::utility::resolver::resolver(const config &config_): m_config(config_) {}

bunsan::utility::resolver::resolver(const boost::property_tree::ptree &ptree):
    resolver(bunsan::config::load<config>(ptree)) {}

bunsan::utility::resolver::resolver(resolver &&res) noexcept
{
    swap(res);
}

bunsan::utility::resolver &bunsan::utility::resolver::operator=(const resolver &res)
{
    resolver tmp(res);
    swap(tmp);
    return *this;
}

bunsan::utility::resolver &bunsan::utility::resolver::operator=(resolver &&res) noexcept
{
    swap(res);
    return *this;
}

void bunsan::utility::resolver::swap(bunsan::utility::resolver::config &a, bunsan::utility::resolver::config &b) noexcept
{
    a.alias.swap(b.alias);
    a.absolute.swap(b.absolute);
    a.path.swap(b.path);
}

void bunsan::utility::resolver::swap(resolver &res) noexcept
{
    if (m_config)
    {
        if (res.m_config)
        {
            swap(m_config.get(), res.m_config.get());
        }
        else
        {
            // let boost::optional deal with that
            m_config.swap(res.m_config);
        }
    }
    else
    {
        if (res.m_config)
        {
            m_config.swap(res.m_config);
        }
        else
        {
            // nothing to do
        }
    }
}

void bunsan::utility::resolver::apply_alias(boost::filesystem::path &name) const
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

void bunsan::utility::resolver::apply_absolute(boost::filesystem::path &name) const
{
    if (!m_config)
        return;
    const auto iter = m_config->absolute.find(name);
    if (iter != m_config->absolute.end())
        name = iter->second;
}

void bunsan::utility::resolver::apply_path(boost::filesystem::path &name) const
{
    if (m_config)
    {
        if (m_config->path.find(name) != m_config->path.end() && name == name.filename())
            name = boost::process::find_executable_in_path(name.string());
    }
    else
    {
        if (name == name.filename())
            name = boost::process::find_executable_in_path(name.string());
    }
}

boost::filesystem::path bunsan::utility::resolver::find_executable(const boost::filesystem::path &exe) const
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

boost::filesystem::path bunsan::utility::resolver::find_library(const boost::filesystem::path &lib) const
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
