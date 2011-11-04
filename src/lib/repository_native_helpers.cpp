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
	boost::property_tree::read_info(source_resource(package, value(config::name::file::index)).generic_string(), ptree);
}

void bunsan::pm::repository::native::read_checksum(const entry &package, boost::property_tree::ptree &ptree)
{
	boost::property_tree::read_info(source_resource(package, value(config::name::file::checksum)).generic_string(), ptree);
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

bunsan::pm::repository::native::native(const boost::property_tree::ptree &config_): config(config_){}

bunsan::pm::depends bunsan::pm::repository::native::read_depends(const entry &package)
{
	boost::property_tree::ptree index;
	read_index(package, index);
	depends deps;
	for (const auto &i: index.get_child(pm::index::package))
		deps.package.insert(std::make_pair(i.first, i.second.get_value<std::string>()));
	for (const auto &i: index.get_child(pm::index::source::self))
		deps.source.self.insert(std::make_pair(i.first, i.second.get_value<std::string>()));
	for (const auto &i: index.get_child(pm::index::source::import::package))
		deps.source.import.package.insert(std::make_pair(i.first, i.second.get_value<std::string>()));
	for (const auto &i: index.get_child(pm::index::source::import::source))
		deps.source.import.source.insert(std::make_pair(i.first, i.second.get_value<std::string>()));
	return deps;
}

namespace
{
	void check_dir(const boost::filesystem::path &dir)
	{
		if (!dir.is_absolute())
			throw std::runtime_error("you have to use absolute path, but "+dir.generic_string()+" was used");
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

