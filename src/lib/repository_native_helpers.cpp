#include "repository_native.hpp"

#include <bunsan/pm/config.hpp>

#include <bunsan/config/cast.hpp>
#include <bunsan/enable_error_info.hpp>
#include <bunsan/logging/legacy.hpp>
#include <bunsan/filesystem/operations.hpp>

#include <boost/property_tree/info_parser.hpp>

bunsan::pm::snapshot_entry bunsan::pm::repository::native::read_checksum(const entry &package)
{
    boost::property_tree::ptree ptree;
    boost::property_tree::read_info(source_resource(package, m_config.name.file.checksum).string(), ptree);
    return bunsan::config::load<snapshot_entry>(ptree);
}

std::string bunsan::pm::repository::native::remote_resource(const entry &package, const boost::filesystem::path &name)
{
    return package.remote_resource(m_config.repository_url, name);
}

boost::filesystem::path bunsan::pm::repository::native::source_resource(const entry &package, const std::string &name)
{
    return package.local_resource(m_config.dir.source, name);
}

boost::filesystem::path bunsan::pm::repository::native::package_resource(const entry &package, const std::string &name)
{
    return package.local_resource(m_config.dir.package, name);
}

bunsan::pm::repository::native::native(const pm::config &config_):
    m_config(config_),
    m_resolver(m_config.utility.resolver)
{
    auto &utility = m_config.utility;

    if (!(cache_archiver = utility.cache_archiver.instance_optional(m_resolver)))
        BOOST_THROW_EXCEPTION(invalid_configuration_cache_archiver_error() <<
                              invalid_configuration_cache_archiver_error::utility_type(utility.cache_archiver.type));

    if (!(source_archiver = utility.source_archiver.instance_optional(m_resolver)))
        BOOST_THROW_EXCEPTION(invalid_configuration_source_archiver_error() <<
                              invalid_configuration_source_archiver_error::utility_type(utility.source_archiver.type));

    if (!(builder = utility.builder.instance_optional(m_resolver)))
        BOOST_THROW_EXCEPTION(invalid_configuration_builder_error() <<
                              invalid_configuration_builder_error::utility_type(utility.builder.type));

    if (!(fetcher = utility.fetcher.instance_optional(m_resolver)))
        BOOST_THROW_EXCEPTION(invalid_configuration_fetcher_error() <<
                              invalid_configuration_fetcher_error::utility_type(utility.fetcher.type));
}

void bunsan::pm::repository::native::write_snapshot(const boost::filesystem::path &path, const snapshot &snapshot_)
{
    boost::property_tree::write_info(path.string(), bunsan::config::save<boost::property_tree::ptree>(snapshot_));
}

bunsan::pm::snapshot bunsan::pm::repository::native::read_snapshot(const boost::filesystem::path &path)
{
    BUNSAN_EXCEPTIONS_WRAP_BEGIN()
    {
        boost::property_tree::ptree ptree;
        boost::property_tree::read_info(path.string(), ptree);
        return bunsan::config::load<snapshot>(ptree);
    }
    BUNSAN_EXCEPTIONS_WRAP_END()
}

bunsan::pm::index bunsan::pm::repository::native::read_index(const entry &package)
{
    BUNSAN_EXCEPTIONS_WRAP_BEGIN()
    {
        return index(source_resource(package, m_config.name.file.index));
    }
    BUNSAN_EXCEPTIONS_WRAP_END_ERROR_INFO(error::action(__func__) << error::package(package))
}

namespace
{
    void check_dir(const boost::filesystem::path &dir)
    {
        if (!dir.is_absolute())
            BOOST_THROW_EXCEPTION(bunsan::pm::invalid_configuration_error() <<
                                  bunsan::pm::invalid_configuration_error::path(dir) <<
                                  bunsan::pm::invalid_configuration_error::message(
                                      "You have to specify absolute path"));
        SLOG("checking " << dir);
        if (!boost::filesystem::is_directory(dir))
        {
            if (!boost::filesystem::exists(dir))
            {
                SLOG("directory " << dir << " was not found");
            }
            else
            {
                SLOG(dir << " is not a directory: starting recursive remove");
                boost::filesystem::remove_all(dir);
            }
            SLOG("trying to create " << dir);
            boost::filesystem::create_directory(dir);
            DLOG(created);
        }
    }
}

bool bunsan::pm::repository::native::build_outdated(const entry &package, const snapshot &snapshot_)
{
    const boost::filesystem::path snp = package_resource(package, m_config.name.file.build_snapshot);
    const boost::filesystem::path build = package_resource(package, m_config.name.file.build);
    if (!boost::filesystem::exists(snp) || !boost::filesystem::exists(build))
        return true;
    return snapshot_ != read_snapshot(snp);
}

bool bunsan::pm::repository::native::installation_outdated(const entry &package, const snapshot &snapshot_)
{
    const boost::filesystem::path snp = package_resource(package, m_config.name.file.installation_snapshot);
    const boost::filesystem::path installation = package_resource(package, m_config.name.file.installation);
    if (!boost::filesystem::exists(snp) || !boost::filesystem::exists(installation))
        return true;
    return snapshot_ != read_snapshot(snp);
}

void bunsan::pm::repository::native::check_dirs()
{
    DLOG(checking directories);
    check_dir(m_config.dir.source);
    check_dir(m_config.dir.package);
    check_dir(m_config.dir.tmp);
    DLOG(checked);
}

void bunsan::pm::repository::native::clean()
{
    DLOG(trying to clean cache);
    filesystem::reset_dir(m_config.dir.source);
    filesystem::reset_dir(m_config.dir.package);
    filesystem::reset_dir(m_config.dir.tmp);
    DLOG(cleaned);
}
