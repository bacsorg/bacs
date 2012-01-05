#include "repository_native.hpp"
#include "bunsan/pm/index.hpp"
#include "bunsan/pm/config.hpp"

#include "bunsan/util.hpp"

std::string bunsan::pm::repository::native::value(const std::string &key)
{
	return config.get<std::string>(key);
}

void bunsan::pm::repository::native::read_index(const entry &package, boost::property_tree::ptree &ptree)
{
	boost::property_tree::read_info(source_resource(package, value(config::name::file::index)).string(), ptree);
}

void bunsan::pm::repository::native::read_checksum(const entry &package, boost::property_tree::ptree &ptree)
{
	boost::property_tree::read_info(source_resource(package, value(config::name::file::checksum)).string(), ptree);
}

std::string bunsan::pm::repository::native::remote_resource(const entry &package, const std::string &name)
{
	return package.remote_resource(value(config::repository_url), name);
}

boost::filesystem::path bunsan::pm::repository::native::source_resource(const entry &package, const std::string &name)
{
	return package.local_resource(value(config::dir::source), name);
}

boost::filesystem::path bunsan::pm::repository::native::package_resource(const entry &package, const std::string &name)
{
	return package.local_resource(value(config::dir::package), name);
}

std::multimap<boost::filesystem::path, std::string> bunsan::pm::repository::native::sources(const entry &package)
{
	return read_depends(package).source.self;
}

void bunsan::pm::repository::native::pack(const bunsan::executor &packer_, const boost::filesystem::path &source, const boost::filesystem::path &destination)
{
	bunsan::executor packer = packer_;
	packer.current_path(source.parent_path())(source.filename(), destination);// FIXME encapsulation fault
}

bunsan::pm::repository::native::native(const boost::property_tree::ptree &config_): config(config_){}

void bunsan::pm::repository::native::write_snapshot(const boost::filesystem::path &path, const std::map<entry, boost::property_tree::ptree> &snapshot)
{
	boost::property_tree::ptree snapshot_;
	for (const auto &i: snapshot)
		snapshot_.push_back(boost::property_tree::ptree::value_type(i.first.name(), i.second));
	boost::property_tree::write_info(path.string(), snapshot_);
}

std::map<bunsan::pm::entry, boost::property_tree::ptree> bunsan::pm::repository::native::read_snapshot(const boost::filesystem::path &path)
{
	std::map<entry, boost::property_tree::ptree> snapshot;
	boost::property_tree::ptree snapshot_;
	boost::property_tree::read_info(path.string(), snapshot_);
	for (const auto &i: snapshot_)
	{
		auto iter = snapshot.find(i.first);
		if (iter!=snapshot.end())
			BOOST_ASSERT(iter->second==i.second);
		else
			snapshot[i.first] = i.second;
	}
	return snapshot;
}

bunsan::pm::depends bunsan::pm::repository::native::read_depends(const entry &package)
{
	try
	{
		boost::property_tree::ptree index;
		read_index(package, index);
		depends deps(index);
		return deps;
	}
	catch (std::exception &e)
	{
		throw pm_error("Unable to read package depends", e);
	}
}

namespace
{
	void check_dir(const boost::filesystem::path &dir)
	{
		if (!dir.is_absolute())
			throw std::runtime_error("you have to use absolute path, but "+dir.string()+" was used");
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
}

bool bunsan::pm::repository::native::build_outdated(const entry &package, const std::map<entry, boost::property_tree::ptree> &snapshot)
{
	boost::filesystem::path snp = package_resource(package, value(config::name::file::build_snapshot));
	boost::filesystem::path build = package_resource(package, value(config::name::file::build));
	if (!boost::filesystem::exists(snp) || !boost::filesystem::exists(build))
		return true;
	std::map<entry, boost::property_tree::ptree> snapshot_ = read_snapshot(snp);
	return snapshot!=snapshot_;
}

bool bunsan::pm::repository::native::installation_outdated(const entry &package, const std::map<entry, boost::property_tree::ptree> &snapshot)
{
	boost::filesystem::path snp = package_resource(package, value(config::name::file::installation_snapshot));
	boost::filesystem::path installation = package_resource(package, value(config::name::file::installation));
	if (!boost::filesystem::exists(snp) || !boost::filesystem::exists(installation))
		return true;
	std::map<entry, boost::property_tree::ptree> snapshot_ = read_snapshot(snp);
	return snapshot!=snapshot_;
}

void bunsan::pm::repository::native::check_dirs()
{
	DLOG(checking directories);
	check_dir(value(config::dir::source));
	check_dir(value(config::dir::package));
	check_dir(value(config::dir::tmp));
	DLOG(checked);
}

void bunsan::pm::repository::native::clean()
{
	DLOG(trying to clean cache);
	bunsan::reset_dir(value(config::dir::source));
	bunsan::reset_dir(value(config::dir::package));
	bunsan::reset_dir(value(config::dir::tmp));
	DLOG(cleaned);
}

