#include "../repository_native.hpp"

#include <bunsan/pm/config.hpp>

#include <bunsan/config/cast.hpp>
#include <bunsan/enable_error_info.hpp>
#include <bunsan/filesystem/operations.hpp>
#include <bunsan/logging/legacy.hpp>

#include <boost/property_tree/info_parser.hpp>

bunsan::pm::snapshot_entry bunsan::pm::repository::native::read_checksum(const entry &package)
{
    try
    {
        boost::property_tree::ptree ptree;
        boost::property_tree::read_info(source_resource(package, m_config.remote.format.name.get_checksum()).string(), ptree);
        return bunsan::config::load<snapshot_entry>(ptree);
    }
    catch (std::exception &)
    {
        BOOST_THROW_EXCEPTION(native_read_checksum_error() <<
                              native_read_checksum_error::package(package) <<
                              enable_nested_current());
    }
}

std::string bunsan::pm::repository::native::remote_resource(const entry &package, const boost::filesystem::path &name)
{
    return package.remote_resource(m_config.remote.url, name);
}

boost::filesystem::path bunsan::pm::repository::native::source_resource(const entry &package, const boost::filesystem::path &name)
{
    return package.local_resource(m_config.cache.get_source(), name);
}

boost::filesystem::path bunsan::pm::repository::native::package_resource(const entry &package, const boost::filesystem::path &name)
{
    return package.local_resource(m_config.cache.get_package(), name);
}

bunsan::pm::repository::native::native(const pm::config &config_):
    m_config(config_),
    m_resolver(m_config.local_system.resolver)
{
    if (!(cache_archiver = m_config.cache.archiver.instance_optional(m_resolver)))
        BOOST_THROW_EXCEPTION(invalid_configuration_cache_archiver_error() <<
                              invalid_configuration_cache_archiver_error::utility_type(m_config.cache.archiver.type));

    if (!(source_archiver = m_config.remote.format.archiver.instance_optional(m_resolver)))
        BOOST_THROW_EXCEPTION(invalid_configuration_source_archiver_error() <<
                              invalid_configuration_source_archiver_error::utility_type(m_config.remote.format.archiver.type));

    const auto builder_iter = m_config.build.builders.find(m_config.remote.format.builder);
    if (builder_iter == m_config.build.builders.end())
        BOOST_THROW_EXCEPTION(invalid_configuration_builder_not_found_error() <<
                              invalid_configuration_builder_not_found_error::builder(m_config.remote.format.builder));

    if (!(builder = builder_iter->second.instance_optional(m_resolver)))
        BOOST_THROW_EXCEPTION(invalid_configuration_builder_error() <<
                              invalid_configuration_builder_error::utility_type(builder_iter->second.type));

    if (!(fetcher = m_config.remote.fetcher.instance_optional(m_resolver)))
        BOOST_THROW_EXCEPTION(invalid_configuration_fetcher_error() <<
                              invalid_configuration_fetcher_error::utility_type(m_config.remote.fetcher.type));
}

void bunsan::pm::repository::native::write_snapshot(const boost::filesystem::path &path, const snapshot &snapshot_)
{
    try
    {
        boost::property_tree::write_info(path.string(), bunsan::config::save<boost::property_tree::ptree>(snapshot_));
    }
    catch (std::exception &)
    {
        BOOST_THROW_EXCEPTION(native_write_snapshot_error() <<
                              native_write_snapshot_error::path(path) <<
                              enable_nested_current());
    }
}

bunsan::pm::snapshot bunsan::pm::repository::native::read_snapshot(const boost::filesystem::path &path)
{
    try
    {
        boost::property_tree::ptree ptree;
        boost::property_tree::read_info(path.string(), ptree);
        return bunsan::config::load<snapshot>(ptree);
    }
    catch (std::exception &)
    {
        BOOST_THROW_EXCEPTION(native_read_snapshot_error() <<
                              native_read_snapshot_error::path(path) <<
                              enable_nested_current());
    }
}

bunsan::pm::index bunsan::pm::repository::native::read_index(const entry &package)
{
    try
    {
        return index(source_resource(package, m_config.remote.format.name.get_index()));
    }
    catch (std::exception &)
    {
        BOOST_THROW_EXCEPTION(native_read_index_error() <<
                              native_read_index_error::package(package) <<
                              enable_nested_current());
    }
}

bool bunsan::pm::repository::native::build_outdated(const entry &package, const snapshot &snapshot_)
{
    try
    {
        const boost::filesystem::path snp = package_resource(package, m_config.cache.name.get_build_snapshot());
        const boost::filesystem::path build = package_resource(package, m_config.cache.name.get_build_archive());
        if (!boost::filesystem::exists(snp) || !boost::filesystem::exists(build))
            return true;
        return snapshot_ != read_snapshot(snp);
    }
    catch (std::exception &)
    {
        BOOST_THROW_EXCEPTION(native_build_outdated_error() <<
                              native_build_outdated_error::package(package) <<
                              enable_nested_current());
    }
}

bool bunsan::pm::repository::native::installation_outdated(const entry &package, const snapshot &snapshot_)
{
    try
    {
        const boost::filesystem::path snp = package_resource(package, m_config.cache.name.get_installation_snapshot());
        const boost::filesystem::path installation = package_resource(package, m_config.cache.name.get_installation_archive());
        if (!boost::filesystem::exists(snp) || !boost::filesystem::exists(installation))
            return true;
        return snapshot_ != read_snapshot(snp);
    }
    catch (std::exception &)
    {
        BOOST_THROW_EXCEPTION(native_installation_outdated_error() <<
                              native_installation_outdated_error::package(package) <<
                              enable_nested_current());
    }
}
