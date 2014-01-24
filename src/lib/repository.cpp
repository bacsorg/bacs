#include <bunsan/pm/repository.hpp>

#include "repository/builder.hpp"
#include "repository/cache.hpp"
#include "repository/distributor.hpp"
#include "repository/extractor.hpp"
#include "repository/local_system.hpp"
#include "repository/merge_maps.hpp"

#include <bunsan/config/cast.hpp>
#include <bunsan/logging/legacy.hpp>

#include <boost/algorithm/string/classification.hpp>
#include <boost/algorithm/string/predicate.hpp>
#include <boost/assert.hpp>
#include <boost/filesystem.hpp>
#include <boost/interprocess/sync/scoped_lock.hpp>
#include <boost/interprocess/sync/sharable_lock.hpp>

#include <deque>
#include <map>
#include <memory>
#include <queue>
#include <set>
#include <stdexcept>
#include <string>
#include <vector>

#include <cstdlib>

bunsan::pm::repository::repository(const pm::config &config_):
    m_config(config_)
{
    // note: repository reference is stored,
    // it must be noncopyable and non-movable or references must be updated
    // FIXME order of initialization matters
    if (m_config.local_system)
        m_local_system.reset(new local_system(*this, *m_config.local_system));
    if (m_config.remote)
        m_distributor.reset(new distributor(*this, *m_config.remote));
    if (m_config.build)
        m_builder.reset(new builder(*this, *m_config.build));
    if (m_config.cache)
        m_cache.reset(new cache(*this, *m_config.cache));
    if (m_config.extract)
        m_extractor.reset(new extractor(*this, *m_config.extract));
}

bunsan::pm::repository::repository(const boost::property_tree::ptree &config_):
    repository(bunsan::config::load<pm::config>(config_)) {}

const bunsan::pm::config &bunsan::pm::repository::config() const
{
    return m_config;
}

void bunsan::pm::repository::create(const boost::filesystem::path &source, bool strip)
{
    try
    {
        SLOG("creating source package from " << source << " with" << (strip ? "" : "out") << " stripping");
        distributor_().create(source, strip);
    }
    catch (std::exception &)
    {
        BOOST_THROW_EXCEPTION(create_error() <<
                              create_error::source(source) <<
                              create_error::strip(strip) <<
                              enable_nested_current());
    }
}

void bunsan::pm::repository::create_recursively(
    const boost::filesystem::path &root, bool strip)
{
    try
    {
        SLOG("recursively creating source packages from " <<
             root << " with" << (strip ? "" : "out") << " stripping");
        distributor_().create_recursively(root, strip);
    }
    catch (std::exception &)
    {
        BOOST_THROW_EXCEPTION(create_error() <<
                              create_error::root(root) <<
                              create_error::strip(strip) <<
                              enable_nested_current());
    }
}

void bunsan::pm::repository::extract(
    const bunsan::pm::entry &package, const boost::filesystem::path &destination)
{
    try
    {
        SLOG("Attempt to extract \"" << package << "\" to " << destination);
        const auto cache_guard = cache_().lock();
        DLOG(trying to update);
        update(package);
        DLOG(trying to extract);
        extractor_().extract(package, destination);
    }
    catch (std::exception &)
    {
        BOOST_THROW_EXCEPTION(extract_error() <<
                              extract_error::package(package) <<
                              extract_error::destination(destination) <<
                              enable_nested_current());
    }
}

void bunsan::pm::repository::install(
    const entry &package, const boost::filesystem::path &destination)
{
    try
    {
        SLOG("Attempt to install \"" << package << "\" to " << destination);
        const auto cache_guard = cache_().lock();
        DLOG(trying to update);
        update(package);
        DLOG(trying to install);
        extractor_().install(package, destination);
    }
    catch (std::exception &)
    {
        BOOST_THROW_EXCEPTION(install_error() <<
                              install_error::package(package) <<
                              install_error::destination(destination) <<
                              enable_nested_current());
    }
}

void bunsan::pm::repository::update(
    const entry &package, const boost::filesystem::path &destination)
{
    try
    {
        SLOG("Attempt to update \"" << package << "\" installation in " << destination);
        const auto cache_guard = cache_().lock();
        DLOG(trying to update package);
        update(package);
        DLOG(trying to update installation);
        extractor_().update(package, destination);
    }
    catch (std::exception &)
    {
        BOOST_THROW_EXCEPTION(update_error() <<
                              update_error::package(package) <<
                              update_error::destination(destination) <<
                              enable_nested_current());
    }
}

void bunsan::pm::repository::update(const entry &package,
                                    const boost::filesystem::path &destination,
                                    const std::time_t &lifetime)
{
    try
    {
        if (need_update(package, destination, lifetime))
            update(package, destination);
        else
            SLOG("Skipping \"" << package << "\" update since lifetime = " <<
                 lifetime << " has not passed since previous attempt.");
    }
    catch (std::exception &)
    {
        BOOST_THROW_EXCEPTION(update_error() <<
                              update_error::package(package) <<
                              update_error::destination(destination) <<
                              update_error::lifetime(lifetime) <<
                              enable_nested_current());
    }
}

bool bunsan::pm::repository::need_update(const entry &package,
                                         const boost::filesystem::path &destination,
                                         const std::time_t &lifetime)
{
    try
    {
        return extractor_().need_update(destination, lifetime);
    }
    catch (std::exception &)
    {
        BOOST_THROW_EXCEPTION(need_update_error() <<
                              need_update_error::package(package) <<
                              need_update_error::destination(destination) <<
                              need_update_error::lifetime(lifetime) <<
                              enable_nested_current());
    }
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
    cache_().verify_and_repair();
    DLOG(starting build);
    update_meta_tree(package);
    std::map<stage, bool> updated;
    std::map<stage, snapshot> snapshot_cache;
    std::set<stage> in;
    snapshot current_snapshot;
    update_package_depends(
        stage(package, stage_type::installation),
        updated,
        in,
        current_snapshot,
        snapshot_cache
    );
}

void bunsan::pm::repository::update_meta_tree(const entry &package)
{
    std::set<entry> visited;
    const std::function<void (const entry &)> update_meta =
        [this, &visited, &update_meta](const entry &package)
        {
            if (visited.find(package) == visited.end())
            {
                distributor_().update_meta(package);
                visited.insert(package);
                const index deps = cache_().read_index(package);
                for (const entry &i: deps.all())
                    update_meta(i);
            }
        };
    update_meta(package);
}

bool bunsan::pm::repository::update_package_depends(
    const stage &package,
    std::map<stage, bool> &updated,
    std::set<stage> &in,
    snapshot &current_snapshot,
    std::map<stage, snapshot> &snapshot_cache)
{
    SLOG("starting \"" << package.first << "\" (" <<
         stage_type_name[static_cast<int>(package.second)] << ") " << __func__);
    if (in.find(package) != in.end())
    {
        BOOST_THROW_EXCEPTION(circular_dependencies_error() <<
                              circular_dependencies_error::package(package.first.name()));
    }
    // update current_snapshot
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
    const index deps = cache_().read_index(package.first);
    switch (package.second)
    {
    case stage_type::installation:
        {
            for (const auto &i: deps.package.import.package)
            {
                snapshot snapshot_;
                const bool ret = update_package_depends(
                    stage(i.second, stage_type::installation),
                    updated,
                    in,
                    snapshot_,
                    snapshot_cache
                );
                upd = upd || ret;
                merge_maps(current_snapshot, snapshot_);
            }
            for (const auto &i: deps.package.import.source)
            {
                snapshot snapshot_;
                const bool ret = update_package_depends(
                    stage(i.second, stage_type::source),
                    updated,
                    in,
                    snapshot_,
                    snapshot_cache
                );
                upd = upd || ret;
                merge_maps(current_snapshot, snapshot_);
            }
            if (deps.source.empty())
            {
                snapshot snapshot_;
                /// \note build is not needed because no sources provided
                const bool ret = update_package_depends(
                    stage(package.first, stage_type::build_empty),
                    updated,
                    in,
                    snapshot_,
                    snapshot_cache
                );
                upd = upd || ret;
                merge_maps(current_snapshot, snapshot_);
            }
            else
            {
                /// \note source is updated by build
                snapshot snapshot_;
                const bool ret = update_package_depends(
                    stage(package.first, stage_type::build),
                    updated,
                    in,
                    snapshot_,
                    snapshot_cache
                );
                upd = upd || ret;
                merge_maps(current_snapshot, snapshot_);
            }
            upd = upd || cache_().installation_outdated(package.first, current_snapshot);
            if (upd)
            {
                builder_().build_installation(package.first);
                BOOST_ASSERT(!cache_().installation_outdated(package.first, current_snapshot));
            }
        }
        break;
    case stage_type::build:
        {
            {
                snapshot snapshot_;
                const bool ret = update_package_depends(
                    stage(package.first, stage_type::source),
                    updated,
                    in,
                    snapshot_,
                    snapshot_cache
                );
                upd = upd || ret;
                merge_maps(current_snapshot, snapshot_);
            }
            upd = upd || cache_().build_outdated(package.first, current_snapshot);
            if (upd)
            {
                builder_().build(package.first);
                BOOST_ASSERT(!cache_().build_outdated(package.first, current_snapshot));
            }
        }
        break;
    case stage_type::build_empty:
        {
            {
                snapshot snapshot_;
                const bool ret = update_package_depends(
                    stage(package.first, stage_type::source),
                    updated,
                    in,
                    snapshot_,
                    snapshot_cache
                );
                upd = upd || ret;
                merge_maps(current_snapshot, snapshot_);
            }
            upd = upd || cache_().build_outdated(package.first, current_snapshot);
            if (upd)
            {
                builder_().build_empty(package.first);
                BOOST_ASSERT(!cache_().build_outdated(package.first, current_snapshot));
            }
        }
        break;
    case stage_type::source:
        {
            for (const auto &i: deps.source.import.package)
            {
                snapshot snapshot_;
                const bool ret = update_package_depends(
                    stage(i.second, stage_type::installation),
                    updated,
                    in,
                    snapshot_,
                    snapshot_cache
                );
                upd = upd || ret;
                merge_maps(current_snapshot, snapshot_);
            }
            for (const auto &i: deps.source.import.source)
            {
                snapshot snapshot_;
                const bool ret = update_package_depends(
                    stage(i.second, stage_type::source),
                    updated,
                    in,
                    snapshot_,
                    snapshot_cache
                );
                upd = upd || ret;
                merge_maps(current_snapshot, snapshot_);
            }
            distributor_().update_sources(package.first);
            {
                const snapshot_entry checksum = cache_().read_checksum(package.first);
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

void bunsan::pm::repository::clean_cache()
{
    try
    {
        const auto cache_guard = cache_().lock();
        cache_().clean();
    }
    catch (std::exception &)
    {
        BOOST_THROW_EXCEPTION(clean_error() <<
                              enable_nested_current());
    }
}

std::string bunsan::pm::repository::version()
{
#ifdef BUNSAN_PM_VERSION
    return #BUNSAN_PM_VERSION;
#else
    return "bunsan::pm version " __DATE__ " " __TIME__;
#endif
}

bunsan::pm::repository::~repository()
{
    // implicit ptr destructors
}

#define BUNSAN_PM_GETTER(X, CFG) \
    bunsan::pm::repository::X &bunsan::pm::repository::X##_() \
    { \
        if (!m_##X) \
            BOOST_THROW_EXCEPTION(\
                null_##X##_error() << \
                null_##X##_error::message("\"" CFG "\" was not initialized")); \
        return *m_##X; \
    }

BUNSAN_PM_GETTER(local_system, "local_system")
BUNSAN_PM_GETTER(cache, "cache")
BUNSAN_PM_GETTER(distributor, "remote")
BUNSAN_PM_GETTER(builder, "build")
BUNSAN_PM_GETTER(extractor, "extract")

#undef BUNSAN_PM_GETTER
