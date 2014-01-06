#include <bunsan/config.hpp>

#include "repository_native.hpp"

#include <bunsan/pm/checksum.hpp>
#include <bunsan/pm/config.hpp>

#include <bunsan/enable_error_info.hpp>
#include <bunsan/filesystem/operations.hpp>
#include <bunsan/logging/legacy.hpp>
#include <bunsan/tempfile.hpp>

#include <boost/property_tree/info_parser.hpp>
#include <boost/property_tree/ptree.hpp>

#include <stdexcept>

void bunsan::pm::repository::native::build(const entry &package)
{
    const tempfile build_dir = tempfile::in_dir(m_config.local_system.build_dir);
    filesystem::reset_dir(build_dir.path());
    unpack(package, build_dir.path());
    pack(package, build_dir.path());
}

namespace
{
    bool outdated(const boost::filesystem::path &file, const std::string &checksum)
    {
        return !boost::filesystem::exists(file) || bunsan::pm::checksum(file) != checksum;
    }
    void load(const bunsan::utility::fetcher_ptr &fetcher,
              const std::string &source,
              const boost::filesystem::path &file,
              const std::string &checksum)
    {
        if (outdated(file, checksum))
        {
            fetcher->fetch(source, file);
            if (outdated(file, checksum))
                BOOST_THROW_EXCEPTION(bunsan::pm::error() << // TODO exception name should better reflect error
                                      bunsan::pm::error::message("Error loading file, wrong checksum") <<
                                      bunsan::pm::error::path(file));
        }
    }
}

void bunsan::pm::repository::native::update_index(const entry &package)
{
    try
    {
        SLOG("starting \"" << package << "\" " << __func__);
        const tempfile checksum_tmp = m_config.local_system.tmp_file ?
            tempfile::from_model(*m_config.local_system.tmp_file) : tempfile::unique();
        try
        {
            fetcher->fetch(remote_resource(package, m_config.remote.format.name.get_checksum()), checksum_tmp.path());
        }
        catch (std::exception &)
        {
            BOOST_THROW_EXCEPTION(native_fetch_index_error() <<
                                  native_fetch_index_error::path(checksum_tmp.path()) <<
                                  native_fetch_index_error::message("no such package in repository") <<
                                  enable_nested_current());
        }
        boost::filesystem::path output = package.local_resource(m_config.cache.get_source());
        boost::filesystem::create_directories(output);
        boost::filesystem::copy_file(checksum_tmp.path(), output / m_config.remote.format.name.get_checksum(),
                                     boost::filesystem::copy_option::overwrite_if_exists);
        load(fetcher,
             remote_resource(package, m_config.remote.format.name.get_index()),
             output / m_config.remote.format.name.get_index(),
             read_checksum(package).at(m_config.remote.format.name.get_index()));
    }
    catch (std::exception &)
    {
        BOOST_THROW_EXCEPTION(native_update_index_error() <<
                              native_update_index_error::package(package) <<
                              enable_nested_current());
    }
}

void bunsan::pm::repository::native::fetch_source(const entry &package)
{
    try
    {
        SLOG("starting \"" << package << "\" " << __func__);
        const std::string src_sfx = m_config.remote.format.name.suffix.archive;
        const boost::filesystem::path output = package.local_resource(m_config.cache.get_source());
        for (const std::string &src_name: read_index(package).sources())
        {
            const std::string src = src_name + src_sfx;
            load(fetcher,
                 remote_resource(package, src),
                 output / src,
                 read_checksum(package).at(src_name));
        }
    }
    catch (std::exception &)
    {
        BOOST_THROW_EXCEPTION(native_fetch_source_error() <<
                              native_fetch_source_error::package(package) <<
                              enable_nested_current());
    }
}

namespace
{
    void merge_dir(const boost::filesystem::path &source, const boost::filesystem::path &destination)
    {
        SLOG("merging dirs: source = " << source << ", destination = " << destination);
        for (boost::filesystem::directory_iterator i(source), end; i != end; ++i)
        {
            const boost::filesystem::path src = i->path(), dst = destination / i->path().filename();
            if (boost::filesystem::is_directory(src) && boost::filesystem::is_directory(dst))
                merge_dir(src, dst);
            else
                boost::filesystem::rename(src, dst);
        }
    }
    void extract(const bunsan::utility::archiver_ptr &extractor, const boost::filesystem::path &source,
        const boost::filesystem::path &destination, const boost::filesystem::path &subsource=boost::filesystem::path())
    {
        boost::filesystem::create_directories(destination);
        const bunsan::tempfile tmp = bunsan::tempfile::in_dir(destination);
        bunsan::filesystem::reset_dir(tmp.path());
        extractor->unpack(source, tmp.path());
        merge_dir(tmp.path() / subsource, destination);
    }
}

void bunsan::pm::repository::native::unpack_source(
    const entry &package, const boost::filesystem::path &destination, snapshot &snapshot_)
{
    SLOG("starting " << package << " import unpack");
    const index deps = read_index(package);
    // extract sources
    for (const auto &i: deps.source.self)
        ::extract(
            source_archiver,
            source_resource(package, i.second + m_config.remote.format.name.suffix.archive),
            destination / i.first, i.second
        );
    // dump package checksum into snapshot
    {
        const auto iter = snapshot_.find(package);
        const snapshot_entry checksum = read_checksum(package);
        if (iter != snapshot_.end())
            BOOST_ASSERT(iter->second == checksum);
        else
            snapshot_[package.name()] = checksum;
    }
    // extract source imports
    for (const auto &i: deps.source.import.source)
        unpack_source(i.second, destination / i.first, snapshot_);
    // extract package imports
    for (const auto &i: deps.source.import.package)
    {
        SLOG("starting \"" << package << "\" import extraction");
        const boost::filesystem::path snp = package_resource(i.second, m_config.cache.name.get_installation_snapshot());
        extract_installation(i.second, destination / i.first, false);
        merge_maps(snapshot_, read_snapshot(snp));
    }
}

void bunsan::pm::repository::native::unpack(const entry &package, const boost::filesystem::path &build_dir)
{
    try
    {
        SLOG("starting \"" << package << "\" " << __func__);
        const boost::filesystem::path src = build_dir / m_config.build.name.dir.get_source();
        const boost::filesystem::path build = build_dir / m_config.build.name.dir.get_build();
        const boost::filesystem::path installation = build_dir / m_config.build.name.dir.get_installation();
        const boost::filesystem::path snp = build_dir/ m_config.cache.name.get_build_snapshot();
        // create/clean directories
        filesystem::reset_dir(src);
        filesystem::reset_dir(build);
        filesystem::reset_dir(installation);
        // unpack source
        snapshot snapshot_;
        unpack_source(package, src, snapshot_);
        write_snapshot(snp, snapshot_);
    }
    catch (std::exception &)
    {
        BOOST_THROW_EXCEPTION(native_unpack_error() <<
                              native_unpack_error::package(package) <<
                              native_unpack_error::build_dir(build_dir) <<
                              enable_nested_current());
    }
}

void bunsan::pm::repository::native::pack(const entry &package, const boost::filesystem::path &build_dir)
{
    try
    {
        SLOG("starting \"" << package << "\" " << __func__);
        const boost::filesystem::path snp = build_dir / m_config.cache.name.get_build_snapshot();
        builder->install(
            build_dir / m_config.build.name.dir.get_source(),
            build_dir / m_config.build.name.dir.get_build(),
            build_dir / m_config.build.name.dir.get_installation());
        cache_archiver->pack(
            build_dir / m_config.cache.name.get_build_archive(),
            build_dir / m_config.build.name.dir.get_installation());
        boost::filesystem::create_directories(package.local_resource(m_config.cache.get_package()));
        boost::filesystem::copy_file(
            build_dir / m_config.cache.name.get_build_archive(),
            package_resource(package, m_config.cache.name.get_build_archive()),
            boost::filesystem::copy_option::overwrite_if_exists);
        boost::filesystem::copy_file(snp, package_resource(package, m_config.cache.name.get_build_snapshot()),
            boost::filesystem::copy_option::overwrite_if_exists);
    }
    catch (std::exception &)
    {
        BOOST_THROW_EXCEPTION(native_pack_error() <<
                              native_pack_error::package(package) <<
                              native_pack_error::build_dir(build_dir) <<
                              enable_nested_current());
    }
}

void bunsan::pm::repository::native::build_empty(const entry &package)
{
    try
    {
        SLOG("starting \"" << package << "\" " << __func__);
        const tempfile build_dir = tempfile::in_dir(m_config.local_system.build_dir);
        boost::filesystem::create_directory(build_dir.path());
        // create empty archive
        boost::filesystem::create_directory(build_dir.path() / m_config.build.name.dir.get_installation());
        cache_archiver->pack(
            build_dir.path() / m_config.cache.name.get_build_archive(),
            build_dir.path() / m_config.build.name.dir.get_installation());
        // create build cache
        boost::filesystem::create_directories(package.local_resource(m_config.cache.get_package()));
        boost::filesystem::copy_file(
            build_dir.path() / m_config.cache.name.get_build_archive(),
            package_resource(package, m_config.cache.name.get_build_archive()),
            boost::filesystem::copy_option::overwrite_if_exists);
        const snapshot snapshot_ = {
            {package, read_checksum(package)}
        };
        write_snapshot(package_resource(package, m_config.cache.name.get_build_snapshot()), snapshot_);
    }
    catch (std::exception &)
    {
        BOOST_THROW_EXCEPTION(native_build_empty_error() <<
                              native_build_empty_error::package(package) <<
                              enable_nested_current());
    }
}

void bunsan::pm::repository::native::extract_build(const entry &package, const boost::filesystem::path &destination)
{
    try
    {
        SLOG("starting \"" << package << "\" " << __func__);
        filesystem::reset_dir(destination);
        ::extract(
            cache_archiver,
            package_resource(package, m_config.cache.name.get_build_archive()),
            destination,
            m_config.build.name.dir.get_installation()
        );
    }
    catch (std::exception &)
    {
        BOOST_THROW_EXCEPTION(native_extract_build_error() <<
                              native_extract_build_error::package(package) <<
                              native_extract_build_error::destination(destination) <<
                              enable_nested_current());
    }
}

void bunsan::pm::repository::native::build_installation(const entry &package)
{
    try
    {
        SLOG("starting " << package << " " << __func__);
        const tempfile build_dir = tempfile::in_dir(m_config.local_system.build_dir);
        filesystem::reset_dir(build_dir.path());
        boost::filesystem::path install_dir = build_dir.path() / m_config.build.name.dir.get_installation();
        // unpack
        extract_build(package, install_dir);
        snapshot snapshot_ = read_snapshot(package_resource(package, m_config.cache.name.get_build_snapshot()));
        const index deps = read_index(package);
        for (const auto &i: deps.package.self)
            ::extract(
                source_archiver,
                source_resource(package, i.second + m_config.remote.format.name.suffix.archive),
                install_dir / i.first, i.second
            );
        for (const auto &i: deps.package.import.source)
            unpack_source(i.second, install_dir / i.first, snapshot_);
        for (const auto &i: deps.package.import.package)
        {
            extract_installation(i.second, install_dir / i.first, false);
            merge_maps(
                snapshot_,
                read_snapshot(package_resource(i.second, m_config.cache.name.get_installation_snapshot()))
            );
        }
        // save snapshot
        write_snapshot(build_dir.path() / m_config.cache.name.get_installation_snapshot(), snapshot_);
        // pack
        cache_archiver->pack(
            build_dir.path() / m_config.cache.name.get_installation_archive(),
            install_dir);
        boost::filesystem::copy_file(
            build_dir.path() / m_config.cache.name.get_installation_archive(),
            package_resource(package, m_config.cache.name.get_installation_archive()),
            boost::filesystem::copy_option::overwrite_if_exists);
        boost::filesystem::copy_file(
            build_dir.path() / m_config.cache.name.get_installation_snapshot(),
            package_resource(package, m_config.cache.name.get_installation_snapshot()),
            boost::filesystem::copy_option::overwrite_if_exists);
    }
    catch (std::exception &)
    {
        BOOST_THROW_EXCEPTION(native_build_installation_error() <<
                              native_build_installation_error::package(package) <<
                              enable_nested_current());
    }
}

void bunsan::pm::repository::native::extract_installation(
    const entry &package, const boost::filesystem::path &destination, bool reset)
{
    try
    {
        SLOG("starting " << package << " " << __func__);
        if (reset)
            filesystem::reset_dir(destination);
        else
            boost::filesystem::create_directories(destination);
        ::extract(
            cache_archiver,
            package_resource(package, m_config.cache.name.get_installation_archive()),
            destination,
            m_config.build.name.dir.get_installation()
        );
    }
    catch (std::exception &)
    {
        BOOST_THROW_EXCEPTION(native_extract_installation_error() <<
                              native_extract_installation_error::package(package) <<
                              native_extract_installation_error::destination(destination) <<
                              native_extract_installation_error::reset(reset) <<
                              enable_nested_current());
    }
}

void bunsan::pm::repository::native::install_installation(
    const entry &package, const boost::filesystem::path &destination)
{
    try
    {
        SLOG("starting " << package << " " << __func__);
        const boost::filesystem::path meta = destination / m_config.extract.installation.meta;
        const boost::filesystem::path snp = package_resource(package, m_config.cache.name.get_installation_snapshot());
        boost::filesystem::path dst = destination;
        if (m_config.extract.installation.data)
            dst /= m_config.extract.installation.data.get();
        bunsan::filesystem::reset_dir(destination);
        extract_installation(package, dst);
        if (boost::filesystem::exists(meta))
        {
            BOOST_ASSERT(!m_config.extract.installation.data);
            BOOST_THROW_EXCEPTION(installation_meta_exists_error() <<
                                  installation_meta_exists_error::meta(meta));
        }
        boost::filesystem::copy_file(snp, meta, boost::filesystem::copy_option::fail_if_exists);
        boost::filesystem::last_write_time(meta, std::time(nullptr));
    }
    catch (std::exception &)
    {
        BOOST_THROW_EXCEPTION(native_install_installation_error() <<
                              native_install_installation_error::package(package) <<
                              native_install_installation_error::destination(destination) <<
                              enable_nested_current());
    }
}

void bunsan::pm::repository::native::update_installation(
    const entry &package, const boost::filesystem::path &destination)
{
    try
    {
        SLOG("starting " << package << " " << __func__);
        const boost::filesystem::path meta = destination / m_config.extract.installation.meta;
        const boost::filesystem::path snp = package_resource(package, m_config.cache.name.get_installation_snapshot());
        boost::optional<snapshot> snapshot_;
        if (boost::filesystem::is_regular_file(meta))
        {
            try
            {
                snapshot_ = read_snapshot(meta);
            }
            catch (std::exception &)
            {
                SLOG("warning: unable to read snapshot from " << meta << ", falling back to outdated.");
            }
        }
        if (!snapshot_ || snapshot_.get() != read_snapshot(snp))
        {
            SLOG("\"" << package << "\" installation at " << destination << " is outdated, updating...");
            install_installation(package, destination);
        }
        else
        {
            boost::filesystem::last_write_time(meta, std::time(nullptr));
        }
    }
    catch (std::exception &)
    {
        BOOST_THROW_EXCEPTION(native_update_installation_error() <<
                              native_update_installation_error::package(package) <<
                              native_update_installation_error::destination(destination) <<
                              enable_nested_current());
    }
}

bool bunsan::pm::repository::native::need_update_installation(
    const boost::filesystem::path &destination, const std::time_t &lifetime)
{
    try
    {
        SLOG("starting " << destination << " " << __func__);
        const boost::filesystem::path meta = destination / m_config.extract.installation.meta;
        return !boost::filesystem::is_regular_file(meta) ||
               std::time(nullptr) > lifetime + boost::filesystem::last_write_time(meta);
    }
    catch (std::exception &)
    {
        BOOST_THROW_EXCEPTION(native_need_update_installation_error() <<
                              native_need_update_installation_error::destination(destination) <<
                              native_need_update_installation_error::lifetime(lifetime) <<
                              enable_nested_current());
    }
}
