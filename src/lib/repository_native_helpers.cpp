#include "repository_native.hpp"
#include "repository_config.hpp"

#include "bunsan/util.hpp"

std::string bunsan::pm::repository::native::value(const std::string &key)
{
	return config.get<std::string>(key);
}

void bunsan::pm::repository::native::read_index(const entry &package, boost::property_tree::ptree &ptree)
{
	boost::property_tree::read_info(local_resource(package, value(name_file_index)).native(), ptree);
}

void bunsan::pm::repository::native::read_checksum(const entry &package, boost::property_tree::ptree &ptree)
{
	boost::property_tree::read_info(local_resource(package, value(name_file_checksum)).native(), ptree);
}

std::string bunsan::pm::repository::native::remote_resource(const entry &package, const std::string &name)
{
	return package.remote_resource(value(repository_url), name);
}

boost::filesystem::path bunsan::pm::repository::native::local_resource(const entry &package, const std::string &name)
{
	return package.local_resource(value(dir_source), name);
}

bunsan::pm::repository::native::native(const boost::property_tree::ptree &config_): config(config_){}

namespace
{
	void check_dir(const boost::filesystem::path &dir)
	{
		if (!dir.is_absolute())
			throw std::runtime_error("you have to use absolute path, but "+dir.native()+" was used");
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
	check_dir(value(dir_source));
	check_dir(value(dir_package));
	check_dir(value(dir_tmp));
	DLOG(checked);
}

void bunsan::pm::repository::native::clean()
{
	DLOG(trying to clean cache);
	bunsan::reset_dir(value(dir_source));
	bunsan::reset_dir(value(dir_package));
	bunsan::reset_dir(value(dir_tmp));
	DLOG(cleaned);
}

