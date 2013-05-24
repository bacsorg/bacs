#include "repository_native.hpp"

#include "bunsan/pm/repository.hpp"
#include "bunsan/pm/config.hpp"

#include "bunsan/config/cast.hpp"
#include "bunsan/enable_error_info.hpp"
#include "bunsan/logging/legacy.hpp"

#include <stdexcept>
#include <string>
#include <set>
#include <map>
#include <deque>
#include <queue>
#include <vector>
#include <memory>

#include <cstdlib>
#include <cassert>

#include <boost/algorithm/string/classification.hpp>
#include <boost/algorithm/string/predicate.hpp>
#include <boost/interprocess/sync/scoped_lock.hpp>
#include <boost/interprocess/sync/sharable_lock.hpp>
#include <boost/filesystem.hpp>
#include <boost/assert.hpp>

bunsan::pm::repository::repository(const pm::config &config_): ntv(nullptr), m_config(config_)
{
    if (m_config.lock.global)
        m_flock.reset(new bunsan::interprocess::file_lock(m_config.lock.global->c_str()));
    ntv = new native(m_config);
}

bunsan::pm::repository::repository(const boost::property_tree::ptree &config_):
    repository(bunsan::config::load<pm::config>(config_)) {}

const bunsan::pm::config &bunsan::pm::repository::config() const
{
    return m_config;
}

void bunsan::pm::repository::create(const boost::filesystem::path &source, bool strip)
{
    SLOG("creating source package from " << source << " with" << (strip ? "" : "out") << " stripping");
    ntv->create(source, strip);
}

void bunsan::pm::repository::create_recursively(const boost::filesystem::path &root, bool strip)
{
    SLOG("recursively creating source packages from " << root << " with" << (strip ? "" : "out") << " stripping");
    ntv->create_recursively(root, strip);
}

namespace
{
    void require_lock(bool has, const char *func)
    {
        using bunsan::pm::invalid_configuration_key_error;
        if (!has)
            BOOST_THROW_EXCEPTION(invalid_configuration_key_error() <<
                                  invalid_configuration_key_error::path("config.lock.global") <<
                                  invalid_configuration_key_error::action(func));
    }
}

void bunsan::pm::repository::extract(const bunsan::pm::entry &package, const boost::filesystem::path &destination)
{
    require_lock(static_cast<bool>(m_flock), __func__);
    SLOG("Attempt to extract \"" << package << "\" to " << destination);
    boost::interprocess::scoped_lock<bunsan::interprocess::file_lock> lk(*m_flock);
    DLOG(trying to update);
    update(package);
    DLOG(trying to extract);
    ntv->extract_installation(package, destination);
}

void bunsan::pm::repository::install(const entry &package, const boost::filesystem::path &destination)
{
    require_lock(static_cast<bool>(m_flock), __func__);
    SLOG("Attempt to install \"" << package << "\" to " << destination);
    boost::interprocess::scoped_lock<bunsan::interprocess::file_lock> lk(*m_flock);
    DLOG(trying to update);
    update(package);
    DLOG(trying to install);
    ntv->install_installation(package, destination);
}

void bunsan::pm::repository::update(const entry &package, const boost::filesystem::path &destination)
{
    require_lock(static_cast<bool>(m_flock), __func__);
    SLOG("Attempt to update \"" << package << "\" installation in " << destination);
    boost::interprocess::scoped_lock<bunsan::interprocess::file_lock> lk(*m_flock);
    DLOG(trying to update package);
    update(package);
    DLOG(trying to update installation);
    ntv->update_installation(package, destination);
}

void bunsan::pm::repository::update(const entry &package,
                                    const boost::filesystem::path &destination,
                                    const std::time_t &lifetime)
{
    if (need_update(package, destination, lifetime))
        update(package, destination);
    else
        SLOG("Skipping \"" << package << "\" update since lifetime = " <<
             lifetime << " has not passed since previous attempt.");
}

bool bunsan::pm::repository::need_update(const entry &package,
                                         const boost::filesystem::path &destination,
                                         const std::time_t &lifetime)
{
    BUNSAN_EXCEPTIONS_WRAP_BEGIN()
    {
        return ntv->need_update_installation(destination, lifetime);
    }
    BUNSAN_EXCEPTIONS_WRAP_END_ERROR_INFO(error::package(package))
}

enum class bunsan::pm::repository::stage_type: int
{
    installation,
    build,
    build_empty,
    source
};

namespace
{
    const char *stage_type_name[] =
    {
        "installation",
        "build",
        "build_empty",
        "source"
    };
}

void bunsan::pm::repository::update(const bunsan::pm::entry &package)
{
    SLOG("updating \"" << package << "\"");
    ntv->check_dirs();
    DLOG(starting build);
    update_index_tree(package);
    std::map<stage, bool> updated;
    std::map<stage, snapshot> snapshot_cache;
    std::set<stage> in;
    snapshot current_snapshot;
    update_package_depends(stage(package, stage_type::installation), updated, in, current_snapshot, snapshot_cache);
}

void bunsan::pm::repository::update_index_tree(const entry &package)
{
    std::set<entry> visited;
    std::function<void(const entry &)> update_index =
        [this, &visited, &update_index](const entry &package)
        {
            if (visited.find(package) == visited.end())
            {
                ntv->update_index(package);
                visited.insert(package);
                const index deps = ntv->read_index(package);
                for (const entry &i: deps.all())
                    update_index(i);
            }
        };
    update_index(package);
}

bool bunsan::pm::repository::update_package_depends(
    const stage &package,
    std::map<stage, bool> &updated,
    std::set<stage> &in,
    snapshot &current_snapshot,
    std::map<stage, snapshot> &snapshot_cache)
{
    SLOG("starting \"" << package.first << "\" (" << stage_type_name[static_cast<int>(package.second)] << ") " << __func__);
    if (in.find(package) != in.end())
        BOOST_THROW_EXCEPTION(circular_dependencies() << circular_dependencies::package(package.first.name()));
    {
        const auto iter = updated.find(package);
        if (iter != updated.end())
        {
            current_snapshot = snapshot_cache.at(package);
            return iter->second;
        }
    }
    in.insert(package);
    bool upd = false;
    const index deps = ntv->read_index(package.first);
    switch (package.second)
    {
    case stage_type::installation:
        {
            for (const auto &i: deps.package.import.package)
            {
                snapshot snapshot_;
                const bool ret = update_package_depends(stage(i.second, stage_type::installation), updated, in, snapshot_, snapshot_cache);
                upd = upd || ret;
                merge_maps(current_snapshot, snapshot_);
            }
            for (const auto &i: deps.package.import.source)
            {
                snapshot snapshot_;
                const bool ret = update_package_depends(stage(i.second, stage_type::source), updated, in, snapshot_, snapshot_cache);
                upd = upd || ret;
                merge_maps(current_snapshot, snapshot_);
            }
            if (deps.source.empty())
            {
                snapshot snapshot_;
                /// \note build is not needed because no sources provided
                const bool ret = update_package_depends(stage(package.first, stage_type::build_empty), updated, in, snapshot_, snapshot_cache);
                upd = upd || ret;
                merge_maps(current_snapshot, snapshot_);
            }
            else
            {
                /// \note source is updated by build
                snapshot snapshot_;
                const bool ret = update_package_depends(stage(package.first, stage_type::build), updated, in, snapshot_, snapshot_cache);
                upd = upd || ret;
                merge_maps(current_snapshot, snapshot_);
            }
            upd = upd || ntv->installation_outdated(package.first, current_snapshot);
            if (upd)
            {
                ntv->build_installation(package.first);
                BOOST_ASSERT(!ntv->installation_outdated(package.first, current_snapshot));
            }
        }
        break;
    case stage_type::build:
        {
            {
                snapshot snapshot_;
                const bool ret = update_package_depends(stage(package.first, stage_type::source), updated, in, snapshot_, snapshot_cache);
                upd = upd || ret;
                merge_maps(current_snapshot, snapshot_);
            }
            upd = upd || ntv->build_outdated(package.first, current_snapshot);
            if (upd)
            {
                ntv->build(package.first);
                BOOST_ASSERT(!ntv->build_outdated(package.first, current_snapshot));
            }
        }
        break;
    case stage_type::build_empty:
        {
            {
                snapshot snapshot_;
                const bool ret = update_package_depends(stage(package.first, stage_type::source), updated, in, snapshot_, snapshot_cache);
                upd = upd || ret;
                merge_maps(current_snapshot, snapshot_);
            }
            upd = upd || ntv->build_outdated(package.first, current_snapshot);
            if (upd)
            {
                ntv->build_empty(package.first);
                BOOST_ASSERT(!ntv->build_outdated(package.first, current_snapshot));
            }
        }
        break;
    case stage_type::source:
        {
            for (const auto &i: deps.source.import.package)
            {
                snapshot snapshot_;
                const bool ret = update_package_depends(stage(i.second, stage_type::installation), updated, in, snapshot_, snapshot_cache);
                upd = upd || ret;
                merge_maps(current_snapshot, snapshot_);
            }
            for (const auto &i: deps.source.import.source)
            {
                snapshot snapshot_;
                const bool ret = update_package_depends(stage(i.second, stage_type::source), updated, in, snapshot_, snapshot_cache);
                upd = upd || ret;
                merge_maps(current_snapshot, snapshot_);
            }
            // we will always try to fetch source
            // native object will download source only if it is outdated or does not exist
            ntv->fetch_source(package.first);
            {
                const snapshot_entry checksum = ntv->read_checksum(package.first);
                const auto iter = current_snapshot.find(package.first);
                if (iter != current_snapshot.end())
                    BOOST_ASSERT(iter->second == checksum);
                else
                    current_snapshot[package.first] = checksum;
            }
        }
        break;
    default:
        BOOST_ASSERT(false);
    }
    in.erase(package);
    snapshot_cache[package] = current_snapshot;
    return updated[package] = upd;
}

void bunsan::pm::repository::clean()
{
    require_lock(static_cast<bool>(m_flock), __func__);
    boost::interprocess::scoped_lock<bunsan::interprocess::file_lock> lk(*m_flock);
    ntv->clean();
}

bunsan::pm::repository::~repository()
{
    delete ntv;
}
