#include "bunsan/config.hpp"

#include "repository_native.hpp"

#include "bunsan/pm/checksum.hpp"
#include "bunsan/pm/config.hpp"

#include "bunsan/enable_error_info.hpp"
#include "bunsan/logging/legacy.hpp"
#include "bunsan/filesystem/operations.hpp"
#include "bunsan/tempfile.hpp"

#include <stdexcept>

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/info_parser.hpp>
#include <boost/format.hpp>
#include <boost/filesystem.hpp>
#include <boost/filesystem/fstream.hpp>

void bunsan::pm::repository::native::build(const entry &package)
{
    const tempfile build_dir = tempfile::in_dir(m_config.dir.tmp);
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
    void load(const bunsan::utility::fetcher_ptr &fetcher, const std::string &source, const boost::filesystem::path &file, const std::string &checksum)
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
    BUNSAN_EXCEPTIONS_WRAP_BEGIN()
    {
        SLOG("starting \"" << package << "\" " << __func__);
        const tempfile checksum_tmp = tempfile::from_model(m_config.name.file.tmp);
        BUNSAN_EXCEPTIONS_WRAP_BEGIN()
        {
            fetcher->fetch(remote_resource(package, m_config.name.file.checksum), checksum_tmp.path());
        }
        BUNSAN_EXCEPTIONS_WRAP_END_ERROR_INFO(error::action("fetch index") <<
                                              error::message("no such package in repository") <<
                                              error::package(package))
        boost::filesystem::path output = package.local_resource(m_config.dir.source);
        boost::filesystem::create_directories(output);
        boost::filesystem::copy_file(checksum_tmp.path(), output / m_config.name.file.checksum,
                                     boost::filesystem::copy_option::overwrite_if_exists);
        load(fetcher,
             remote_resource(package, m_config.name.file.index),
             output / m_config.name.file.index,
             read_checksum(package).at(m_config.name.file.index));
    }
    BUNSAN_EXCEPTIONS_WRAP_END_ERROR_INFO(error::action(__func__) << error::package(package))
}

void bunsan::pm::repository::native::fetch_source(const entry &package)
{
    BUNSAN_EXCEPTIONS_WRAP_BEGIN()
    {
        SLOG("starting \"" << package << "\" " << __func__);
        const std::string src_sfx = m_config.name.suffix.source_archive;
        const boost::filesystem::path output = package.local_resource(m_config.dir.source);
        for (const std::string &src_name: read_index(package).sources())
        {
            const std::string src = src_name + src_sfx;
            load(fetcher,
                 remote_resource(package, src),
                 output / src,
                 read_checksum(package).at(src_name));
        }
    }
    BUNSAN_EXCEPTIONS_WRAP_END_ERROR_INFO(error::action(__func__) << error::package(package))
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
        ::extract(source_archiver, source_resource(package, i.second + m_config.name.suffix.source_archive), destination / i.first, i.second);
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
        const boost::filesystem::path snp = package_resource(i.second, m_config.name.file.installation_snapshot);
        extract_installation(i.second, destination / i.first, false);
        merge_maps(snapshot_, read_snapshot(snp));
    }
}

void bunsan::pm::repository::native::unpack(const entry &package, const boost::filesystem::path &build_dir)
{
    BUNSAN_EXCEPTIONS_WRAP_BEGIN()
    {
        SLOG("starting \"" << package << "\" " << __func__);
        const boost::filesystem::path src = build_dir / m_config.name.dir.source;
        const boost::filesystem::path build = build_dir / m_config.name.dir.build;
        const boost::filesystem::path installation = build_dir / m_config.name.dir.installation;
        const boost::filesystem::path snp = build_dir/ m_config.name.file.build_snapshot;
        // create/clean directories
        filesystem::reset_dir(src);
        filesystem::reset_dir(build);
        filesystem::reset_dir(installation);
        // unpack source
        snapshot snapshot_;
        unpack_source(package, src, snapshot_);
        write_snapshot(snp, snapshot_);
    }
    BUNSAN_EXCEPTIONS_WRAP_END_ERROR_INFO(error::action(__func__) << error::package(package))
}

void bunsan::pm::repository::native::pack(const entry &package, const boost::filesystem::path &build_dir)
{
    BUNSAN_EXCEPTIONS_WRAP_BEGIN()
    {
        SLOG("starting \"" << package << "\" " << __func__);
        const boost::filesystem::path snp = build_dir / m_config.name.file.build_snapshot;
        builder->install(
            build_dir / m_config.name.dir.source,
            build_dir / m_config.name.dir.build,
            build_dir / m_config.name.dir.installation);
        cache_archiver->pack(
            build_dir / m_config.name.file.build,
            build_dir / m_config.name.dir.installation);
        boost::filesystem::create_directories(package.local_resource(m_config.dir.package));
        boost::filesystem::copy_file(
            build_dir / m_config.name.file.build,
            package_resource(package, m_config.name.file.build),
            boost::filesystem::copy_option::overwrite_if_exists);
        boost::filesystem::copy_file(snp, package_resource(package, m_config.name.file.build_snapshot),
            boost::filesystem::copy_option::overwrite_if_exists);
    }
    BUNSAN_EXCEPTIONS_WRAP_END_ERROR_INFO(error::action(__func__) << error::package(package))
}

void bunsan::pm::repository::native::build_empty(const entry &package)
{
    BUNSAN_EXCEPTIONS_WRAP_BEGIN()
    {
        SLOG("starting \"" << package << "\" " << __func__);
        const tempfile build_dir = tempfile::in_dir(m_config.dir.tmp);
        boost::filesystem::create_directory(build_dir.path());
        // create empty archive
        boost::filesystem::create_directory(build_dir.path() / m_config.name.dir.installation);
        cache_archiver->pack(
            build_dir.path() / m_config.name.file.build,
            build_dir.path() / m_config.name.dir.installation);
        // create build cache
        boost::filesystem::create_directories(package.local_resource(m_config.dir.package));
        boost::filesystem::copy_file(
            build_dir.path() / m_config.name.file.build,
            package_resource(package, m_config.name.file.build),
            boost::filesystem::copy_option::overwrite_if_exists);
        const snapshot snapshot_ = {
            {package, read_checksum(package)}
        };
        write_snapshot(package_resource(package, m_config.name.file.build_snapshot), snapshot_);
    }
    BUNSAN_EXCEPTIONS_WRAP_END_ERROR_INFO(error::action(__func__) << error::package(package))
}

void bunsan::pm::repository::native::extract_build(const entry &package, const boost::filesystem::path &destination)
{
    BUNSAN_EXCEPTIONS_WRAP_BEGIN()
    {
        SLOG("starting \"" << package << "\" " << __func__);
        filesystem::reset_dir(destination);
        ::extract(cache_archiver, package_resource(package, m_config.name.file.build), destination, m_config.name.dir.installation);
    }
    BUNSAN_EXCEPTIONS_WRAP_END_ERROR_INFO(error::action(__func__) << error::package(package))
}

void bunsan::pm::repository::native::build_installation(const entry &package)
{
    BUNSAN_EXCEPTIONS_WRAP_BEGIN()
    {
        SLOG("starting " << package << " " << __func__);
        const tempfile build_dir = tempfile::in_dir(m_config.dir.tmp);
        filesystem::reset_dir(build_dir.path());
        boost::filesystem::path install_dir = build_dir.path() / m_config.name.dir.installation;
        // unpack
        extract_build(package, install_dir);
        snapshot snapshot_ = read_snapshot(package_resource(package, m_config.name.file.build_snapshot));
        const index deps = read_index(package);
        for (const auto &i: deps.package.self)
            ::extract(source_archiver, source_resource(package, i.second + m_config.name.suffix.source_archive), install_dir / i.first, i.second);
        for (const auto &i: deps.package.import.source)
            unpack_source(i.second, install_dir / i.first, snapshot_);
        for (const auto &i: deps.package.import.package)
        {
            extract_installation(i.second, install_dir / i.first, false);
            merge_maps(snapshot_, read_snapshot(package_resource(i.second, m_config.name.file.installation_snapshot)));
        }
        // save snapshot
        write_snapshot(build_dir.path() / m_config.name.file.installation_snapshot, snapshot_);
        // pack
        cache_archiver->pack(
            build_dir.path() / m_config.name.file.installation,
            install_dir);
        boost::filesystem::copy_file(
            build_dir.path() / m_config.name.file.installation,
            package_resource(package, m_config.name.file.installation),
            boost::filesystem::copy_option::overwrite_if_exists);
        boost::filesystem::copy_file(
            build_dir.path() / m_config.name.file.installation_snapshot,
            package_resource(package, m_config.name.file.installation_snapshot),
            boost::filesystem::copy_option::overwrite_if_exists);
    }
    BUNSAN_EXCEPTIONS_WRAP_END_ERROR_INFO(error::action(__func__) << error::package(package))
}

void bunsan::pm::repository::native::extract_installation(const entry &package, const boost::filesystem::path &destination, bool reset)
{
    BUNSAN_EXCEPTIONS_WRAP_BEGIN()
    {
        SLOG("starting " << package << " " << __func__);
        if (reset)
            filesystem::reset_dir(destination);
        else
            boost::filesystem::create_directories(destination);
        ::extract(cache_archiver, package_resource(package, m_config.name.file.installation), destination, m_config.name.dir.installation);
    }
    BUNSAN_EXCEPTIONS_WRAP_END_ERROR_INFO(error::action(__func__) << error::package(package))
}
