#include "cache.hpp"

#include "distributor.hpp"
#include "local_system.hpp"

#include <bunsan/filesystem/fstream.hpp>
#include <bunsan/filesystem/operations.hpp>
#include <bunsan/logging/legacy.hpp>

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

bunsan::pm::repository::cache::lock_guard bunsan::pm::repository::cache::lock()
{
    return lock_guard(m_flock);
}

void bunsan::pm::repository::cache::initialize(const cache_config &config)
{
    try
    {
        if (!config.root.is_absolute())
            BOOST_THROW_EXCEPTION(
                invalid_configuration_relative_path_error() <<
                invalid_configuration_relative_path_error::path(config.root));
        // ignore if directory exists
        boost::filesystem::create_directory(config.root);
        boost::filesystem::create_directory(config.get_source());
        boost::filesystem::create_directory(config.get_package());

        // lock
        if (!config.get_lock().is_absolute())
            BOOST_THROW_EXCEPTION(
                invalid_configuration_relative_path_error() <<
                invalid_configuration_relative_path_error::path(config.get_lock()));
        if (boost::filesystem::exists(config.get_lock()))
        {
            if (!boost::filesystem::is_regular_file(config.get_lock()))
                boost::filesystem::remove(config.get_lock());
        }
        if (!boost::filesystem::exists(config.get_lock()))
        {
            filesystem::ofstream fout(config.get_lock());
            fout.close();
        }
    }
    catch (std::exception &)
    {
        BOOST_THROW_EXCEPTION(cache_initialize_error() <<
                              enable_nested_current());
    }
}

void bunsan::pm::repository::cache::verify_and_repair()
{
    try
    {
        verify_and_repair_directory(m_config.get_source());
        verify_and_repair_directory(m_config.get_package());
    }
    catch (std::exception &)
    {
        BOOST_THROW_EXCEPTION(cache_verify_and_repair_error() <<
                              enable_nested_current());
    }
}

void bunsan::pm::repository::cache::verify_and_repair_directory(
    const boost::filesystem::path &path)
{
    try
    {
        if (!path.is_absolute())
            BOOST_THROW_EXCEPTION(invalid_configuration_relative_path_error() <<
                                  invalid_configuration_relative_path_error::path(path));
        SLOG("checking " << path);
        if (!boost::filesystem::is_directory(path))
        {
            if (!boost::filesystem::exists(path))
            {
                SLOG("directory " << path << " was not found");
            }
            else
            {
                SLOG(path << " is not a directory: starting recursive remove");
                boost::filesystem::remove_all(path);
            }
            if (boost::filesystem::create_directory(path))
                SLOG("created missing " << path << " directory");
        }
    }
    catch (std::exception &)
    {
        BOOST_THROW_EXCEPTION(cache_verify_and_repair_directory_error() <<
                              cache_verify_and_repair_directory_error::path(path) <<
                              enable_nested_current());
    }
}

void bunsan::pm::repository::cache::clean()
{
    try
    {
        verify_and_repair();
        filesystem::reset_dir(m_config.get_source());
        filesystem::reset_dir(m_config.get_package());
    }
    catch (std::exception &)
    {
        BOOST_THROW_EXCEPTION(cache_clean_error() <<
                              enable_nested_current());
    }
}

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

bunsan::pm::snapshot bunsan::pm::repository::cache::read_build_snapshot(
    const entry &package)
{
    try
    {
        return read_snapshot(build_snapshot_path(package));
    }
    catch (std::exception &)
    {
        BOOST_THROW_EXCEPTION(cache_read_build_snapshot_error() <<
                              cache_read_build_snapshot_error::package(package) <<
                              enable_nested_current());
    }
}

bunsan::pm::snapshot bunsan::pm::repository::cache::read_installation_snapshot(
    const entry &package)
{
    try
    {
        return read_snapshot(installation_snapshot_path(package));
    }
    catch (std::exception &)
    {
        BOOST_THROW_EXCEPTION(cache_read_installation_snapshot_error() <<
                              cache_read_installation_snapshot_error::package(package) <<
                              enable_nested_current());
    }
}

void bunsan::pm::repository::cache::unpack_source(
    const entry &package,
    const std::string &source_id,
    const boost::filesystem::path &destination)
{
    try
    {
        distributor_().archiver().unpack(source_path(package, source_id), destination);
    }
    catch (std::exception &)
    {
        BOOST_THROW_EXCEPTION(cache_unpack_source_error() <<
                              cache_unpack_source_error::package(package) <<
                              cache_unpack_source_error::destination(destination) <<
                              enable_nested_current());
    }
}

void bunsan::pm::repository::cache::unpack_build(
    const entry &package,
    const boost::filesystem::path &destination)
{
    try
    {
        m_archiver->unpack(build_archive_path(package), destination);
    }
    catch (std::exception &)
    {
        BOOST_THROW_EXCEPTION(cache_unpack_build_error() <<
                              cache_unpack_build_error::package(package) <<
                              cache_unpack_build_error::destination(destination) <<
                              enable_nested_current());
    }
}

void bunsan::pm::repository::cache::unpack_installation(
    const entry &package,
    const boost::filesystem::path &destination)
{
    try
    {
        m_archiver->unpack(installation_archive_path(package), destination);
    }
    catch (std::exception &)
    {
        BOOST_THROW_EXCEPTION(cache_unpack_installation_error() <<
                              cache_unpack_installation_error::package(package) <<
                              cache_unpack_installation_error::destination(destination) <<
                              enable_nested_current());
    }
}

void bunsan::pm::repository::cache::pack_build(
    const entry &package, const boost::filesystem::path &path)
{
    try
    {
        m_archiver->pack_contents(build_archive_path(package), path);
    }
    catch (std::exception &)
    {
        BOOST_THROW_EXCEPTION(cache_pack_build_error() <<
                              cache_pack_build_error::package(package) <<
                              cache_pack_build_error::path(path) <<
                              enable_nested_current());
    }
}

void bunsan::pm::repository::cache::pack_installation(
    const entry &package, const boost::filesystem::path &path)
{
    try
    {
        m_archiver->pack_contents(installation_archive_path(package), path);
    }
    catch (std::exception &)
    {
        BOOST_THROW_EXCEPTION(cache_pack_installation_error() <<
                              cache_pack_installation_error::package(package) <<
                              cache_pack_installation_error::path(path) <<
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
    return package_file_path(package, m_config.name.get_installation_archive());
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
        boost::filesystem::create_directories(package_root);
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
