#include "cache.hpp"

#include "distributor.hpp"
#include "local_system.hpp"

#include <boost/filesystem/operations.hpp>

bunsan::pm::repository::cache::cache(repository &self, const cache_config &config):
    m_self(self),
    m_config(config),
    m_flock(m_config.get_lock())
{
    if (!(m_archiver = m_config.archiver.instance_optional(local_system_().resolver())))
    {
        BOOST_THROW_EXCEPTION(
            invalid_configuration_cache_archiver_error() <<
            invalid_configuration_cache_archiver_error::utility_type(m_config.archiver.type));
    }
}

//bunsan::pm::repository::cache::lock_guard bunsan::pm::repository::cache::lock();

// void bunsan::pm::repository::cache::validate_and_repair();

// void bunsan::pm::repository::cache::clean();

bunsan::pm::index bunsan::pm::repository::cache::read_index(const entry &package)
{
    try
    {
        return index(index_path(package));
    }
    catch (std::exception &)
    {
        BOOST_THROW_EXCEPTION(cache_read_index_error() <<
                              cache_read_index_error::package(package) <<
                              enable_nested_current());
    }
}

bunsan::pm::snapshot_entry bunsan::pm::repository::cache::read_checksum(const entry &package)
{
    try
    {
        return pm::read_checksum(checksum_path(package));
    }
    catch (std::exception &)
    {
        BOOST_THROW_EXCEPTION(cache_read_checksum_error() <<
                              cache_read_checksum_error::package(package) <<
                              enable_nested_current());
    }
}

bool bunsan::pm::repository::cache::build_outdated(
    const entry &package, const snapshot &snapshot_)
{
    try
    {
        const boost::filesystem::path snp = build_snapshot_path(package);
        const boost::filesystem::path build = build_archive_path(package);
        if (!boost::filesystem::exists(snp) || !boost::filesystem::exists(build))
            return true;
        return snapshot_ != read_snapshot(snp);
    }
    catch (std::exception &)
    {
        BOOST_THROW_EXCEPTION(cache_build_outdated_error() <<
                              cache_build_outdated_error::package(package) <<
                              enable_nested_current());
    }
}

bool bunsan::pm::repository::cache::installation_outdated(
    const entry &package, const snapshot &snapshot_)
{
    try
    {
        const boost::filesystem::path snp = installation_snapshot_path(package);
        const boost::filesystem::path installation = installation_archive_path(package);
        if (!boost::filesystem::exists(snp) || !boost::filesystem::exists(installation))
            return true;
        return snapshot_ != read_snapshot(snp);
    }
    catch (std::exception &)
    {
        BOOST_THROW_EXCEPTION(cache_installation_outdated_error() <<
                              cache_installation_outdated_error::package(package) <<
                              enable_nested_current());
    }
}

boost::filesystem::path bunsan::pm::repository::cache::checksum_path(
    const entry &package)
{
    return source_file_path(package, format().name.get_checksum());
}

boost::filesystem::path bunsan::pm::repository::cache::index_path(
    const entry &package)
{
    return source_file_path(package, format().name.get_index());
}

boost::filesystem::path bunsan::pm::repository::cache::build_archive_path(
    const entry &package)
{
    return package_file_path(package, m_config.name.get_build_archive());
}

boost::filesystem::path bunsan::pm::repository::cache::build_snapshot_path(
    const entry &package)
{
    return package_file_path(package, m_config.name.get_build_snapshot());
}

boost::filesystem::path bunsan::pm::repository::cache::installation_archive_path(
    const entry &package)
{
    return package_file_path(package, m_config.name.get_installation_snapshot());
}

boost::filesystem::path bunsan::pm::repository::cache::installation_snapshot_path(
    const entry &package)
{
    return package_file_path(package, m_config.name.get_installation_snapshot());
}

boost::filesystem::path bunsan::pm::repository::cache::source_path(
    const entry &package, const std::string &source_id)
{
    return source_file_path(package, source_id + format().name.suffix.archive);
}

boost::filesystem::path bunsan::pm::repository::cache::file_path(
    const boost::filesystem::path &root,
    const entry &package,
    const boost::filesystem::path &filename)
{
    try
    {
        if (filename.filename() != filename)
            BOOST_THROW_EXCEPTION(cache_file_path_invalid_filename_error() <<
                                  cache_file_path_invalid_filename_error::filename(filename));
        const boost::filesystem::path package_root = root / package.location();
        boost::filesystem::create_directory(package_root);
        return package_root / filename;
    }
    catch (std::exception &)
    {
        BOOST_THROW_EXCEPTION(cache_file_path_error() <<
                              cache_file_path_error::root(root) <<
                              cache_file_path_error::package(package) <<
                              cache_file_path_error::filename(filename) <<
                              enable_nested_current());
    }
}

boost::filesystem::path bunsan::pm::repository::cache::source_file_path(
    const entry &package, const boost::filesystem::path &filename)
{
    return file_path(m_config.get_source(), package, filename);
}

boost::filesystem::path bunsan::pm::repository::cache::package_file_path(
    const entry &package, const boost::filesystem::path &filename)
{
    return file_path(m_config.get_package(), package, filename);
}

const bunsan::pm::format_config &bunsan::pm::repository::cache::format()
{
    return distributor_().format();
}

bunsan::pm::repository::local_system &bunsan::pm::repository::cache::local_system_()
{
    return m_self.local_system_();
}

bunsan::pm::repository::distributor &bunsan::pm::repository::cache::distributor_()
{
    return m_self.distributor_();
}
