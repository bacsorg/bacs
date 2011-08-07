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
#include <mutex>

#include <cstdlib>
#include <cassert>

#include <boost/algorithm/string/classification.hpp>
#include <boost/algorithm/string/predicate.hpp>
#include <boost/interprocess/sync/scoped_lock.hpp>
#include <boost/interprocess/sync/sharable_lock.hpp>
#include <boost/filesystem.hpp>

#include "bunsan/util.hpp"

bunsan::pm::repository::repository(const boost::property_tree::ptree &config_): ntv(0), config(config_)
{
	DLOG(creating repository instance);
	flock.reset(new boost::interprocess::file_lock(config.get<std::string>("lock.global").c_str()));
	ntv = new native(config);
}

void bunsan::pm::repository::extract(const bunsan::pm::entry &package, const boost::filesystem::path &destination)
{
	SLOG("extract "<<package<<" to "<<destination);
	boost::interprocess::scoped_lock<boost::interprocess::file_lock> lk(*flock);
	boost::interprocess::scoped_lock<std::mutex> lk2(slock);
	DLOG(trying to update);
	update(package);
	DLOG(trying to extract);
	ntv->extract(package, destination);
}

void bunsan::pm::repository::update(const bunsan::pm::entry &package)
{
	SLOG("updating "<<package);
	ntv->check_dirs();
	DLOG(starting build);
	update_index_tree(package);
	std::set<entry> updated, in;
	update_depends(package, updated, in);
}

void bunsan::pm::repository::update_index_tree(const entry &package)
{
	std::set<entry> updated, visited;
	std::function<void(const entry &)> update_imports_index =
		[ntv, &updated, &update_imports_index](const entry &package)
		{
			if (updated.find(package)==updated.end())
			{
				ntv->update_index(package);
				updated.insert(package);
				for (const auto &i: ntv->imports(package))
					update_imports_index(i.second);
			}
		};
	std::function<void(const entry &)> update_depends_index =
		[ntv, &visited, &update_depends_index, &update_imports_index](const entry &package)
		{
			if (visited.find(package)==visited.end())
			{
				update_imports_index(package);
				visited.insert(package);
				for (const auto &i: ntv->depends(package))
					update_depends_index(package);
			}
		};
	update_depends_index(package);
}

void bunsan::pm::repository::update_imports(const entry &package, std::set<entry> &updated, std::set<entry> &in)
{
	SLOG("starting "<<package<<" "<<__func__);
	if (in.find(package)!=in.end())
		throw std::runtime_error("circular imports starting with \""+package.name()+"\"");
	in.insert(package);
	{
		ntv->fetch_source(package);
		for (const auto &i: ntv->imports(package))
		{
			if (updated.find(i.second)==updated.end())
			{
				update_imports(i.second, updated, in);
				updated.insert(i.second);
			}
		}
	}
	in.erase(package);
}

void bunsan::pm::repository::update_depends(const entry &package, std::set<entry> &updated, std::set<entry> &in)
{
	SLOG("starting "<<package<<" "<<__func__);
	if (in.find(package)!=in.end())
		throw std::runtime_error("circular dependencies starting with \""+package.name()+"\"");
	in.insert(package);
	if (ntv->package_outdated(package))
	{
		SLOG(package<<" is outdated, building all dependencies");
		for (const auto &i: ntv->depends(package))
			update_depends(i.second, updated, in);
		// updates imports
		{
			std::set<entry> in_;
			update_imports(package, updated, in_);
		}
		SLOG("building "<<package);
		ntv->build(package);
	}
	else
		SLOG(package<<" is up to date");
	in.erase(package);
}

void bunsan::pm::repository::clean()
{
	boost::interprocess::scoped_lock<boost::interprocess::file_lock> lk(*flock);
	boost::interprocess::scoped_lock<std::mutex> lk2(slock);
	ntv->clean();
}

bunsan::pm::repository::~repository()
{
	delete ntv;
}

std::mutex bunsan::pm::repository::slock;

