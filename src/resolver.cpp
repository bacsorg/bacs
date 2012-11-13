#include "bunsan/utility/resolver.hpp"
#include "bunsan/utility/error.hpp"

#include <boost/process.hpp>

bunsan::utility::resolver::resolver() {}

bunsan::utility::resolver::resolver(const boost::property_tree::ptree &config):
    m_config(config_type())
{
    for (const auto &i: config)
    {
        if (i.first=="alias")
        {
            for (const auto &j: i.second)
                m_config.get().m_alias[j.first] = j.second.get_value<std::string>();
        }
        else if (i.first=="absolute")
        {
            for (const auto &j: i.second)
                m_config.get().m_absolute[j.first] = j.second.get_value<std::string>();
        }
        else if (i.first=="path")
        {
            for (const auto &j: i.second)
                m_config.get().m_path.insert(j.second.get_value<std::string>());
        }
    }
}

bunsan::utility::resolver::resolver(resolver &&res) noexcept
{
    this->swap(res);
}

bunsan::utility::resolver &bunsan::utility::resolver::operator=(const resolver &res)
{
    resolver tmp(res);
    this->swap(tmp);
    return *this;
}

bunsan::utility::resolver &bunsan::utility::resolver::operator=(resolver &&res) noexcept
{
    this->swap(res);
    return *this;
}

void bunsan::utility::resolver::swap(bunsan::utility::resolver::config_type &a, bunsan::utility::resolver::config_type &b) noexcept
{
    a.m_alias.swap(b.m_alias);
    a.m_absolute.swap(b.m_absolute);
    a.m_path.swap(b.m_path);
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
    auto iter = m_config.get().m_alias.find(name);
    while (iter!=m_config.get().m_alias.end())
    {
        name = iter->second;
        iter = m_config.get().m_alias.find(name);
        if (used.find(name)!=used.end())
            BOOST_THROW_EXCEPTION(bunsan::utility::error("Circular dependencies in alias is prohibited"));
        used.insert(name);
    }
}

void bunsan::utility::resolver::apply_absolute(boost::filesystem::path &name) const
{
    if (!m_config)
        return;
    auto iter = m_config.get().m_absolute.find(name);
    if (iter!=m_config.get().m_absolute.end())
        name = iter->second;
}

void bunsan::utility::resolver::apply_path(boost::filesystem::path &name) const
{
    if (m_config)
    {
        if (m_config.get().m_path.find(name)!=m_config.get().m_path.end() && name==name.filename())
            name = boost::process::find_executable_in_path(name.string());
    }
    else
    {
        if (name==name.filename())
            name = boost::process::find_executable_in_path(name.string());
    }
}

boost::filesystem::path bunsan::utility::resolver::find_executable(const boost::filesystem::path &exe) const
{
    boost::filesystem::path ret = exe;
    apply_alias(ret);
    apply_absolute(ret);
    apply_path(ret);
    return ret;
}

boost::filesystem::path bunsan::utility::resolver::find_library(const boost::filesystem::path &lib) const
{
#warning unimplemented
    return lib;
}
