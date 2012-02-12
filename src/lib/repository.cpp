#include "bunsan/pm/repository.hpp"
#include "bunsan/pm/config.hpp"
#include "repository_native.hpp"

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

#include "bunsan/util.hpp"

bunsan::pm::repository::repository(const boost::property_tree::ptree &config_): ntv(0), config(config_)
{
	DLOG(creating repository instance);
	boost::optional<std::string> lock_file = config_get_optional<std::string>(config::lock::global);
	if (lock_file)
		flock.reset(new boost::interprocess::file_lock(lock_file.get().c_str()));
	ntv = new native(config);
}

void bunsan::pm::repository::create(const boost::filesystem::path &source, bool strip)
{
	SLOG("creating source package from "<<source<<" with"<<(strip?"":"out")<<" stripping");
	ntv->create(source, strip);
}

namespace
{
	void require_lock(bool has, const char *func)
	{
		using bunsan::pm::invalid_configuration_path;
		using std::string;
		namespace config = bunsan::pm::config;
		if (!has)
			BOOST_THROW_EXCEPTION(invalid_configuration_path()
				<<invalid_configuration_path::path(config::lock::global)
				<<bunsan::error::message(string("Unable to call ")+func));
	}
}

void bunsan::pm::repository::extract(const bunsan::pm::entry &package, const boost::filesystem::path &destination)
{
	require_lock(static_cast<bool>(flock), __func__);
	SLOG("extract "<<package<<" to "<<destination);
	boost::interprocess::scoped_lock<boost::interprocess::file_lock> lk(*flock);
	boost::mutex::scoped_lock lk2(slock);
	DLOG(trying to update);
	update(package);
	DLOG(trying to extract);
	ntv->extract_installation(package, destination);
}

enum class bunsan::pm::repository::stage_type: int
{
	installation,
	build,
	source
};

namespace
{
	const char *stage_type_name[] =
	{
		"installation",
		"build",
		"source"
	};
}

void bunsan::pm::repository::update(const bunsan::pm::entry &package)
{
	SLOG("updating "<<package);
	ntv->check_dirs();
	DLOG(starting build);
	update_index_tree(package);
	std::map<stage, bool> updated;
	std::map<stage, std::map<entry, boost::property_tree::ptree>> snapshot_cache;
	std::set<stage> in;
	std::map<entry, boost::property_tree::ptree> snapshot;
	update_package_depends(stage(package, stage_type::installation), updated, in, snapshot, snapshot_cache);
}

void bunsan::pm::repository::update_index_tree(const entry &package)
{
	std::set<entry> visited;
	std::function<void(const entry &)> update_index =
		[ntv, &visited, &update_index](const entry &package)
		{
			if (visited.find(package)==visited.end())
			{
				ntv->update_index(package);
				visited.insert(package);
				depends deps = ntv->read_depends(package);
				for (const auto &i: deps.all())
					update_index(i);
			}
		};
	update_index(package);
}

bool bunsan::pm::repository::update_package_depends(
	const stage &package,
	std::map<stage, bool> &updated,
	std::set<stage> &in,
	std::map<entry, boost::property_tree::ptree> &snapshot,
	std::map<stage, std::map<entry, boost::property_tree::ptree>> &snapshot_cache)
{
	SLOG("starting "<<package.first<<" ("<<stage_type_name[static_cast<int>(package.second)]<<") "<<__func__);
	if (in.find(package)!=in.end())
		BOOST_THROW_EXCEPTION(circular_dependencies()<<circular_dependencies::package(package.first.name()));
	{
		auto iter = updated.find(package);
		if (iter!=updated.end())
		{
			snapshot = snapshot_cache.at(package);
			return iter->second;
		}
	}
	in.insert(package);
	bool upd = false;
	depends deps = ntv->read_depends(package.first);
	switch (package.second)
	{
	case stage_type::installation:
		{
			for (const auto &i: deps.package)
			{
				std::map<entry, boost::property_tree::ptree> snapshot_;
				bool ret = update_package_depends(stage(i.second, stage_type::installation), updated, in, snapshot_, snapshot_cache);
				upd = upd || ret;
				merge_maps(snapshot, snapshot_);
			}
			{
				std::map<entry, boost::property_tree::ptree> snapshot_;
				bool ret = update_package_depends(stage(package.first, stage_type::build), updated, in, snapshot_, snapshot_cache);
				upd = upd || ret;
				merge_maps(snapshot, snapshot_);
			}
			upd = upd || ntv->installation_outdated(package.first, snapshot);
			if (upd)
			{
				ntv->build_installation(package.first);
				BOOST_ASSERT(!ntv->installation_outdated(package.first, snapshot));
			}
		}
		break;
	case stage_type::build:
		{
			{
				std::map<entry, boost::property_tree::ptree> snapshot_;
				bool ret = update_package_depends(stage(package.first, stage_type::source), updated, in, snapshot_, snapshot_cache);
				upd = upd || ret;
				merge_maps(snapshot, snapshot_);
			}
			upd = upd || ntv->build_outdated(package.first, snapshot);
			if (upd)
			{
				ntv->build(package.first);
				BOOST_ASSERT(!ntv->build_outdated(package.first, snapshot));
			}
		}
		break;
	case stage_type::source:
		{
			for (const auto &i: deps.source.import.package)
			{
				std::map<entry, boost::property_tree::ptree> snapshot_;
				bool ret = update_package_depends(stage(i.second, stage_type::installation), updated, in, snapshot_, snapshot_cache);
				upd = upd || ret;
				merge_maps(snapshot, snapshot_);
			}
			for (const auto &i: deps.source.import.source)
			{
				std::map<entry, boost::property_tree::ptree> snapshot_;
				bool ret = update_package_depends(stage(i.second, stage_type::source), updated, in, snapshot_, snapshot_cache);
				upd = upd || ret;
				merge_maps(snapshot, snapshot_);
			}
			// we will always try to fetch source
			// native object will download source only if it is outdated or does not exist
			ntv->fetch_source(package.first);
			{
				boost::property_tree::ptree checksum;
				ntv->read_checksum(package.first, checksum);
				auto iter = snapshot.find(package.first);
				if (iter!=snapshot.end())
					BOOST_ASSERT(iter->second==checksum);
				else
					snapshot[package.first] = checksum;
			}
		}
		break;
	default:
		BOOST_ASSERT(false);
	}
	in.erase(package);
	snapshot_cache[package] = snapshot;
	return updated[package] = upd;
}

void bunsan::pm::repository::clean()
{
	require_lock(static_cast<bool>(flock), __func__);
	boost::interprocess::scoped_lock<boost::interprocess::file_lock> lk(*flock);
	boost::mutex::scoped_lock lk2(slock);
	ntv->clean();
}

bunsan::pm::repository::~repository()
{
	delete ntv;
}

