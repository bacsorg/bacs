#include "bunsan/config.hpp"

#include "repository_native.hpp"

#include "bunsan/pm/checksum.hpp"
#include "bunsan/pm/config.hpp"

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
    tempfile build_dir = tempfile::in_dir(value(config::dir::tmp));
    filesystem::reset_dir(build_dir.path());
    unpack(package, build_dir.path());
    pack(package, build_dir.path());
}

namespace
{
    bool outdated(const boost::filesystem::path &file, const std::string &checksum)
    {
        return !boost::filesystem::exists(file) || bunsan::pm::checksum(file)!=checksum;
    }
    void load(const bunsan::utility::fetcher_ptr &fetcher, const std::string &source, const boost::filesystem::path &file, const std::string &checksum)
    {
        if (outdated(file, checksum))
        {
            fetcher->fetch(source, file);
            if (outdated(file, checksum))
                throw std::runtime_error("Error loading file \""+file.string()+"\": wrong checksum");
        }
    }
}

void bunsan::pm::repository::native::update_index(const entry &package)
{
    try
    {
        SLOG("starting "<<package<<" "<<__func__);
        tempfile checksum_tmp = tempfile::from_model(value(config::name::file::tmp));
        try
        {
            fetcher->fetch(remote_resource(package, value(config::name::file::checksum)), checksum_tmp.path());
        }
        catch (std::exception &e)
        {
            throw pm_error("Unable to download package meta info (no such package in repository)", e);
        }
        boost::filesystem::path output = package.local_resource(value(config::dir::source));
        boost::filesystem::create_directories(output);
        boost::filesystem::copy_file(checksum_tmp.path(), output/value(config::name::file::checksum),
                                     boost::filesystem::copy_option::overwrite_if_exists);
        boost::property_tree::ptree checksum;
        read_checksum(package, checksum);
        load(fetcher,
             remote_resource(package, value(config::name::file::index)),
             output/value(config::name::file::index),
             checksum.get<std::string>(value(config::name::file::index)));
    }
    catch (pm_error &e)
    {
        throw;
    }
    catch (std::exception &e)
    {
        throw pm_error("Unable to update package meta info", e);
    }
}

void bunsan::pm::repository::native::fetch_source(const entry &package)
{
    try
    {
        SLOG("starting "<<package<<" "<<__func__);
        const std::string src_sfx = value(config::suffix::source_archive);
        boost::filesystem::path output = package.remote_resource(value(config::dir::source));
        boost::property_tree::ptree checksum;
        read_checksum(package, checksum);
        for (const auto &i: sources(package))
            load(fetcher,
                 remote_resource(package, i.second+src_sfx),
                 output/(i.second+src_sfx),
                 checksum.get<std::string>(i.second));
    }
    catch (std::exception &e)
    {
        throw pm_error("Unable to fetch package source", e);
    }
}

namespace
{
    void merge_dir(const boost::filesystem::path &source, const boost::filesystem::path &destination)
    {
        SLOG("merging dirs: source = "<<source<<", destination = "<<destination);
        for (auto i = boost::filesystem::directory_iterator(source); i!=boost::filesystem::directory_iterator(); ++i)
        {
            boost::filesystem::path src = i->path(), dst = destination/i->path().filename();
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
        bunsan::tempfile tmp = bunsan::tempfile::in_dir(destination);
        bunsan::filesystem::reset_dir(tmp.path());
        extractor->unpack(source, tmp.path());
        merge_dir(tmp.path()/subsource, destination);
    }
}

void bunsan::pm::repository::native::unpack_source(const entry &package, const boost::filesystem::path &destination,
    std::map<entry, boost::property_tree::ptree> &snapshot)
{
    SLOG("starting "<<package<<" import unpack");
    depends deps = read_depends(package);
    // extract sources
    for (const auto &i: deps.source.self)
        ::extract(source_archiver, source_resource(package, i.second+value(config::suffix::source_archive)), destination/i.first, i.second);
    // dump package checksum into snapshot
    {
        auto iter = snapshot.find(package);
        boost::property_tree::ptree checksum;
        read_checksum(package, checksum);
        if (iter!=snapshot.end())
            BOOST_ASSERT(iter->second==checksum);
        else
            snapshot[package.name()] = checksum;
    }
    // extract source imports
    for (const auto &i: deps.source.import.source)
        unpack_source(i.second, destination/i.first, snapshot);
    // extract package imports
    for (const auto &i: deps.source.import.package)
    {
        SLOG("starting "<<package<<" import extraction");
        boost::filesystem::path snp = package_resource(i.second, value(config::name::file::installation_snapshot));
        extract_installation(i.second, destination/i.first, false);
        std::map<entry, boost::property_tree::ptree> snapshot_ = read_snapshot(snp);
        merge_maps(snapshot, snapshot_);
    }
}

void bunsan::pm::repository::native::unpack(const entry &package, const boost::filesystem::path &build_dir)
{
    try
    {
        SLOG("starting "<<package<<" "<<__func__);
        boost::filesystem::path src = build_dir/value(config::name::dir::source);
        boost::filesystem::path build = build_dir/value(config::name::dir::build);
        boost::filesystem::path installation = build_dir/value(config::name::dir::installation);
        boost::filesystem::path snp = build_dir/value(config::name::file::build_snapshot);
        // create/clean directories
        filesystem::reset_dir(src);
        filesystem::reset_dir(build);
        filesystem::reset_dir(installation);
        // unpack source
        std::map<entry, boost::property_tree::ptree> snapshot_map;
        unpack_source(package, src, snapshot_map);
        write_snapshot(snp, snapshot_map);
    }
    catch (std::exception &e)
    {
        throw pm_error("Unable to unpack package", e);
    }
}

void bunsan::pm::repository::native::pack(const entry &package, const boost::filesystem::path &build_dir)
{
    try
    {
        SLOG("starting "<<package<<" "<<__func__);
        boost::filesystem::path snp = build_dir/value(config::name::file::build_snapshot);
        builder->install(
            build_dir/value(config::name::dir::source),
            build_dir/value(config::name::dir::build),
            build_dir/value(config::name::dir::installation));
        cache_archiver->pack(
            build_dir/value(config::name::file::build),
            build_dir/value(config::name::dir::installation));
        boost::filesystem::create_directories(package.local_resource(value(config::dir::package)));
        boost::filesystem::copy_file(
            build_dir/value(config::name::file::build),
            package_resource(package, value(config::name::file::build)),
            boost::filesystem::copy_option::overwrite_if_exists);
        boost::filesystem::copy_file(snp, package_resource(package, value(config::name::file::build_snapshot)),
            boost::filesystem::copy_option::overwrite_if_exists);
    }
    catch (std::exception &e)
    {
        throw pm_error("Unable to pack package", e);
    }
}

void bunsan::pm::repository::native::extract_build(const entry &package, const boost::filesystem::path &destination)
{
    try
    {
        SLOG("starting "<<package<<" "<<__func__);
        filesystem::reset_dir(destination);
        ::extract(cache_archiver, package_resource(package, value(config::name::file::build)), destination, value(config::name::dir::installation));
    }
    catch (std::exception &e)
    {
        throw pm_error("Unable to extract package build", e);
    }
}

void bunsan::pm::repository::native::build_installation(const entry &package)
{
    try
    {
        SLOG("starting "<<package<<" "<<__func__);
        tempfile build_dir = tempfile::in_dir(value(config::dir::tmp));
        filesystem::reset_dir(build_dir.path());
        boost::filesystem::path install_dir = build_dir.path()/value(config::name::dir::installation);
        // unpack
        extract_build(package, install_dir);
        std::map<entry, boost::property_tree::ptree> snapshot =
            read_snapshot(package_resource(package, value(config::name::file::build_snapshot)));
        depends deps = read_depends(package);
        for (const auto &i: deps.package)
        {
            boost::filesystem::path dest = install_dir/i.first;
            extract_installation(i.second, dest, false);
            std::map<entry, boost::property_tree::ptree> snapshot_ =
                read_snapshot(package_resource(i.second, value(config::name::file::installation_snapshot)));
            merge_maps(snapshot, snapshot_);
        }
        // save snapshot
        write_snapshot(build_dir.path()/value(config::name::file::installation_snapshot), snapshot);
        // pack
        cache_archiver->pack(
            build_dir.path()/value(config::name::file::installation),
            install_dir);
        boost::filesystem::copy_file(
            build_dir.path()/value(config::name::file::installation),
            package_resource(package, value(config::name::file::installation)),
            boost::filesystem::copy_option::overwrite_if_exists);
        boost::filesystem::copy_file(
            build_dir.path()/value(config::name::file::installation_snapshot),
            package_resource(package, value(config::name::file::installation_snapshot)),
            boost::filesystem::copy_option::overwrite_if_exists);
    }
    catch (std::exception &e)
    {
        throw pm_error("Unable to build package installation", e);
    }
}

void bunsan::pm::repository::native::extract_installation(const entry &package, const boost::filesystem::path &destination, bool reset)
{
    try
    {
        SLOG("starting "<<package<<" "<<__func__);
        if (reset)
            filesystem::reset_dir(destination);
        else
            boost::filesystem::create_directories(destination);
        ::extract(cache_archiver, package_resource(package, value(config::name::file::installation)), destination, value(config::name::dir::installation));
    }
    catch (std::exception &e)
    {
        throw pm_error("Unable to extract package installation", e);
    }
}
