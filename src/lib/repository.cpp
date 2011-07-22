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

void bunsan::pm::repository::check_package_name(const std::string &package)
{
	if (!boost::algorithm::all(package, [](char c){return c=='_' || ('0'<=c && c<='9') || ('a'<=c && c<='z') || ('A'<=c && c<='Z');}))
		throw std::runtime_error("illegal package name \""+package+"\"");
}

void bunsan::pm::repository::extract(const std::string &package, const boost::filesystem::path &destination)
{
	check_package_name(package);
	SLOG("extract \""<<package<<"\" to "<<destination);
	boost::interprocess::scoped_lock<boost::interprocess::file_lock> lk(*flock);
	boost::interprocess::scoped_lock<std::mutex> lk2(slock);
	DLOG(trying to update);
	update(package);
	native ntv(config);
	DLOG(trying to extract);
	ntv.extract(package, destination);
}

void bunsan::pm::repository::update(const std::string &package)
{
	SLOG("updating \""<<package<<"\"");
	check_dirs();
	check_cycle(package);
	std::map<std::string, bool> status;
	DLOG(starting build);
	dfs(package, status);
}

bool bunsan::pm::repository::dfs(const std::string &package, std::map<std::string, bool> &status)
{
	native ntv(config);
	std::vector<std::string> deps = ntv.depends(package);
	bool updated = false;
	for (const auto &i: ntv.depends(package))
	{
		if (status.find(i)==status.end())
			status[i] = dfs(i, status);
		updated = updated || status[i];
	}
	SLOG("updated=\""<<updated<<"\"");
	SLOG("starting \""<<package<<"\" update");
	if (ntv.source_outdated(package))
	{
		updated = true;
		ntv.fetch_source(package);
	}
	if (updated || ntv.package_outdated(package))
	{
		updated = true;
		ntv.build(package);
	}
	SLOG("\""<<package<<"\" was "<<(updated?"updated":"not updated"));
	return updated;
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

namespace
{
	enum class state{out, in, visited};
	void check_cycle(const std::string &package, std::map<std::string, state> &status, std::function<std::vector<std::string> (const std::string &)> dget)
	{
		if (status.find(package)==status.end())
			status[package] = state::out;
		std::vector<std::string> deps = dget(package);
		switch (status[package])
		{
		case state::out:
			status[package] = state::in;
			for (const auto &i: deps)
			{
				check_cycle(i, status, dget);
			}
			break;
		case state::in:
			throw std::runtime_error("circular dependencies starting with \""+package+"\"");
			break;
		case state::visited:
			break;
		default:
			assert(false);
		}
		status[package] = state::visited;
	}
}

void bunsan::pm::repository::check_cycle(const std::string &package)
{
	SLOG("trying to find circular dependencies starting with \""<<package<<"\"");
	std::map<std::string, state> status;
	native ntv(config);
	std::set<std::string> updated;// we update meta info once
	::check_cycle(package, status,
		[&updated, &ntv](const std::string &package)
		{
			if (updated.find(package)==updated.end())
			{
				updated.insert(package);
				ntv.update_meta(package);
			}
			return ntv.depends(package);
		});
	DLOG((circular dependencies was not found, that is good!));
}

std::mutex bunsan::pm::repository::slock;

