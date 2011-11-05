#include "bunsan/pm/repository.hpp"
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
	flock.reset(new boost::interprocess::file_lock(config.get<std::string>("lock.global").c_str()));
	ntv = new native(config);
}

void bunsan::pm::repository::create(const boost::filesystem::path &source, bool strip)
{
	SLOG("creating source package from "<<source<<" with"<<(strip?"":"out")<<" stripping");
	ntv->create(source, strip);
}

void bunsan::pm::repository::extract(const bunsan::pm::entry &package, const boost::filesystem::path &destination)
{
	SLOG("extract "<<package<<" to "<<destination);
	boost::interprocess::scoped_lock<boost::interprocess::file_lock> lk(*flock);
	boost::mutex::scoped_lock lk2(slock);
	DLOG(trying to update);
	update(package);
	DLOG(trying to extract);
	ntv->extract_installation(package, destination);
}

namespace bunsan{namespace pm{namespace package
{
	enum {installation, build, source};
}}}

void bunsan::pm::repository::update(const bunsan::pm::entry &package)
{
	SLOG("updating "<<package);
	ntv->check_dirs();
	DLOG(starting build);
	update_index_tree(package);
	std::map<std::pair<entry, int>, bool> updated;
	std::set<std::pair<entry, int>> in;
	std::map<entry, boost::property_tree::ptree> snapshot;
	update_package_depends(std::make_pair(package, package::installation), updated, in, snapshot);
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

namespace
{
	template <typename Key, typename Value>
	void merge_maps(std::map<Key, Value> &a, const std::map<Key, Value> &b)
	{
		for (const auto &i: b)
		{
			auto iter = a.find(i.first);
			if (iter!=a.end())
				BOOST_ASSERT(iter->second==i.second);
			else
				a[i.first] = i.second;
		}
	}
}

bool bunsan::pm::repository::update_package_depends(
	const std::pair<entry, int> &package,
	std::map<std::pair<entry, int>, bool> &updated,
	std::set<std::pair<entry, int>> &in,
	std::map<entry, boost::property_tree::ptree> &snapshot)
{
	SLOG("starting "<<package.first<<" ("<<(package.second?"package":"source")<<") "<<__func__);
	if (in.find(package)!=in.end())
		throw std::runtime_error("circular dependencies starting with \""+package.first.name()+"\"");
	{
		auto iter = updated.find(package);
		if (iter!=updated.end())
			return iter->second;
	}
	in.insert(package);
	bool upd = false;
	depends deps = ntv->read_depends(package.first);
	switch (package.second)
	{
	case package::installation:
		{
			for (const auto &i: deps.package)
			{
				std::map<entry, boost::property_tree::ptree> snapshot_;
				bool ret = update_package_depends(std::make_pair(i.second, package::installation), updated, in, snapshot_);
				upd = upd || ret;
				::merge_maps(snapshot, snapshot_);
			}
			{
				std::map<entry, boost::property_tree::ptree> snapshot_;
				bool ret = update_package_depends(std::make_pair(package.first, package::build), updated, in, snapshot_);
				upd = upd || ret;
				::merge_maps(snapshot, snapshot_);
			}
			upd = upd || ntv->installation_outdated(package.first, snapshot);
			if (upd)
			{
				ntv->build_installation(package.first);
				BOOST_ASSERT(!ntv->installation_outdated(package.first, snapshot));
			}
		}
		break;
	case package::build:
		{
			{
				std::map<entry, boost::property_tree::ptree> snapshot_;
				bool ret = update_package_depends(std::make_pair(package.first, package::source), updated, in, snapshot_);
				upd = upd || ret;
				::merge_maps(snapshot, snapshot_);
			}
			upd = upd || ntv->build_outdated(package.first, snapshot);
			if (upd)
			{
				ntv->build(package.first);
				BOOST_ASSERT(!ntv->build_outdated(package.first, snapshot));
			}
		}
		break;
	case package::source:
		{
			for (const auto &i: deps.source.import.package)
			{
				std::map<entry, boost::property_tree::ptree> snapshot_;
				bool ret = update_package_depends(std::make_pair(i.second, package::installation), updated, in, snapshot_);
				upd = upd || ret;
				::merge_maps(snapshot, snapshot_);
			}
			for (const auto &i: deps.source.import.source)
			{
				std::map<entry, boost::property_tree::ptree> snapshot_;
				bool ret = update_package_depends(std::make_pair(i.second, package::source), updated, in, snapshot_);
				upd = upd || ret;
				::merge_maps(snapshot, snapshot_);
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
					snapshot[package.first] =checksum;
			}
		}
		break;
	default:
		BOOST_ASSERT(false);
	}
	in.erase(package);
	return updated[package] = upd;
}

void bunsan::pm::repository::clean()
{
	boost::interprocess::scoped_lock<boost::interprocess::file_lock> lk(*flock);
	boost::mutex::scoped_lock lk2(slock);
	ntv->clean();
}

bunsan::pm::repository::~repository()
{
	delete ntv;
}

