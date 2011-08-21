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
	std::set<entry> supdated, pupdated, pin;
	update_package_imports(package, supdated, pupdated, pin);
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
				for (const auto &i: ntv->source_imports(package))
					update_index(i.second);
				for (const auto &i: ntv->package_imports(package))
					update_index(i.second);
			}
		};
	update_index(package);
}

void bunsan::pm::repository::update_source_imports(const entry &package, std::set<entry> &supdated, std::set<entry> &pupdated, std::set<entry> &sin, std::set<entry> &pin)
{
	SLOG("starting "<<package<<" "<<__func__);
	if (sin.find(package)!=sin.end())
		throw std::runtime_error("circular source imports starting with \""+package.name()+"\"");
	sin.insert(package);
	{
		if (supdated.find(package)==supdated.end())
		{
			ntv->fetch_source(package);
			supdated.insert(package);
		}
		for (const auto &i: ntv->source_imports(package))
			update_source_imports(i.second, supdated, pupdated, sin, pin);
		for (const auto &i: ntv->package_imports(package))
			update_package_imports(i.second, supdated, pupdated, pin);
	}
	sin.erase(package);
}

void bunsan::pm::repository::update_package_imports(const entry &package, std::set<entry> &supdated, std::set<entry> &pupdated, std::set<entry> &pin)
{
	SLOG("starting "<<package<<" "<<__func__);
	if (pin.find(package)!=pin.end())
		throw std::runtime_error("circular package imports starting with \""+package.name()+"\"");
	pin.insert(package);
	if (pupdated.find(package)==pupdated.end() && ntv->package_outdated(package))
	{
		SLOG(package<<" is outdated, checking all imports");
		{
			std::set<entry> sin;
			update_source_imports(package, supdated, pupdated, sin, pin);
		}
		SLOG("building "<<package);
		ntv->build(package);
	}
	else
		SLOG(package<<" is up to date");
	pupdated.insert(package);
	pin.erase(package);
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

