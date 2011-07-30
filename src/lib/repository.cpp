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

bunsan::pm::repository::repository(const boost::property_tree::ptree &config_): config(config_)
{
	DLOG(creating repository instance);
	flock.reset(new boost::interprocess::file_lock(config.get<std::string>("lock.global").c_str()));
}

void bunsan::pm::repository::extract(const bunsan::pm::entry &package, const boost::filesystem::path &destination)
{
	SLOG("extract "<<package<<" to "<<destination);
	boost::interprocess::scoped_lock<boost::interprocess::file_lock> lk(*flock);
	boost::interprocess::scoped_lock<std::mutex> lk2(slock);
	DLOG(trying to update);
	update(package);
	native ntv(config);
	DLOG(trying to extract);
	ntv.extract(package, destination);
}

void bunsan::pm::repository::update(const bunsan::pm::entry &package)
{
	SLOG("updating "<<package);
	check_dirs();
	DLOG(starting build);
	std::set<entry> updated, in;
	std::map<entry, bool> status;
	update_depends(package, status, updated, in);
}

void bunsan::pm::repository::update_imports(const entry &package, std::set<entry> &updated, std::set<entry> &in)
{
	if (in.find(package)!=in.end())
		throw std::runtime_error("circular imports starting with \""+package.name()+"\"");
	in.insert(package);
	{
		native ntv(config);
		ntv.update_index(package);
		ntv.fetch_source(package);
		for (const auto &i: ntv.imports(package))
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

void bunsan::pm::repository::update_depends(const entry &package, std::map<entry, bool> &status, std::set<entry> &updated, std::set<entry> &in)
{
	auto it = status.find(package);
	if (it==status.end())
	{
		status[package] = false;
		it = status.find(package);
		if (in.find(package)!=in.end())
			throw std::runtime_error("circular dependencies starting with \""+package.name()+"\"");
		in.insert(package);
		{
			std::set<entry> in_;
			update_imports(package, updated, in_);
			native ntv(config);
			for (const auto &i: ntv.depends(package))
			{
				update_depends(i.second, status, updated, in);
				it->second = it->second || status.at(package);
			}
			SLOG("updated=\""<<it->second<<"\"");
			SLOG("starting "<<package<<" update");
			if (it->second || ntv.package_outdated(package))
			{
				ntv.build(package);
				it->second = true;
			}
			SLOG(package<<" was "<<(it->second?"updated":"not updated"));
		}
		in.erase(package);
	}
}

void check_dir(const boost::filesystem::path &dir)
{
	if (!dir.is_absolute())
		throw std::runtime_error("you have to use absolute path, but \""+dir.native()+"\" used");
	SLOG("checking "<<dir);
	if (!boost::filesystem::is_directory(dir))
	{
		if (!boost::filesystem::exists(dir))
		{
			SLOG("directory "<<dir<<" was not found");
		}
		else
		{
			SLOG(dir<<" is not a directory: starting recursive remove");
			boost::filesystem::remove_all(dir);
		}
		SLOG("trying to create "<<dir);
		boost::filesystem::create_directory(dir);
		DLOG(created);
	}
}

void bunsan::pm::repository::check_dirs()
{
	DLOG(checking directories);
	check_dir(config.get<std::string>("dir.source"));
	check_dir(config.get<std::string>("dir.package"));
	check_dir(config.get<std::string>("dir.tmp"));
	DLOG(checked);
}

void bunsan::pm::repository::clean()
{
	DLOG(trying to clean cache);
	boost::interprocess::scoped_lock<boost::interprocess::file_lock> lk(*flock);
	boost::interprocess::scoped_lock<std::mutex> lk2(slock);
	bunsan::reset_dir(config.get<std::string>("dir.source"));
	bunsan::reset_dir(config.get<std::string>("dir.package"));
	bunsan::reset_dir(config.get<std::string>("dir.tmp"));
	DLOG(cleaned);
}

std::mutex bunsan::pm::repository::slock;

