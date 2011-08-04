#include <bunsan/config.hpp>

#include "repository_native.hpp"
#include "repository_config.hpp"

#include <stdexcept>

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/info_parser.hpp>
#include <boost/format.hpp>
#include <boost/filesystem.hpp>
#include <boost/filesystem/fstream.hpp>

#include "bunsan/executor.hpp"
#include "bunsan/util.hpp"
#include "bunsan/tempfile.hpp"

#include "bunsan/pm/checksum.hpp"

void bunsan::pm::repository::native::build(const entry &package)
{
	bunsan::tempfile_ptr build_dir = bunsan::tempfile::in_dir(value(dir_tmp));
	bunsan::reset_dir(build_dir->path());
	unpack(package, build_dir->path());
	configure(package, build_dir->path());
	compile(package, build_dir->path());
	pack(package, build_dir->path());
}

namespace
{
	class pm_error: public std::runtime_error
	{
	public:
		pm_error(const std::string &msg, const std::exception &e): std::runtime_error("Error occured: \""+msg+"\" because \""+e.what()+"\""){}
	};

	bool outdated(const boost::filesystem::path &file, const std::string &checksum)
	{
		return !boost::filesystem::exists(file) || bunsan::pm::checksum(file)!=checksum;
	}
}

void bunsan::pm::repository::native::update_index(const entry &package)
{
	try
	{
		SLOG("starting "<<package<<" "<<__func__);
		bunsan::executor fetcher(config.get_child(command_fetch));
		bunsan::tempfile_ptr checksum_ptr = bunsan::tempfile::from_model(value(name_file_tmp));
		try
		{
			fetcher(remote_resource(package, value(name_file_checksum)), checksum_ptr->path());
		}
		catch (std::exception &e)
		{
			throw pm_error("Unable to download package meta info (no such package in repository)", e);
		}
		boost::filesystem::path output = package.local_resource(value(dir_source));
		boost::filesystem::create_directories(output);
		boost::filesystem::copy_file(checksum_ptr->path(), output/value(name_file_checksum),
			boost::filesystem::copy_option::overwrite_if_exists);
		boost::property_tree::ptree checksum;
		read_checksum(package, checksum);
		if (outdated(output/value(name_file_index), checksum.get<std::string>(value(name_file_index))))
			fetcher(remote_resource(package, value(name_file_index)), output/value(name_file_index));
	}
	catch (pm_error &e)
	{
		throw;
	}
	catch (std::exception &e)
	{
		throw pm_error("Unable to update package meta info", e);
	}
}

void bunsan::pm::repository::native::fetch_source(const entry &package)
{
	try
	{
		SLOG("starting "<<package<<" "<<__func__);
		const std::string src_sfx = value(suffix_src);
		bunsan::executor fetcher(config.get_child(command_fetch));
		boost::filesystem::path output = package.remote_resource(value(dir_source));
		boost::property_tree::ptree index, checksum;
		read_index(package, index);
		read_checksum(package, checksum);
		for (const auto &i: index.get_child(child_sources))
		{
			std::string src = i.second.get_value<std::string>();
			if (outdated(output/(src+src_sfx), checksum.get<std::string>(src)))
				fetcher(remote_resource(package, src+src_sfx), output/(src+src_sfx));
		}
	}
	catch (std::exception &e)
	{
		throw pm_error("Unable to fetch package source", e);
	}
}

namespace
{
	void move_all_from(const boost::filesystem::path &source, const boost::filesystem::path &destination)
	{
		for (auto i = boost::filesystem::directory_iterator(source); i!=boost::filesystem::directory_iterator(); ++i)
		{
			SLOG("moving "<<i->path()<<" to "<<destination/(i->path().filename()));
			boost::filesystem::rename(i->path(), destination/(i->path().filename()));
		}
	}
	void extract(const bunsan::executor &extractor, const boost::filesystem::path &source,
		const boost::filesystem::path &destination, const boost::filesystem::path &subsource=boost::filesystem::path())
	{
		bunsan::tempfile_ptr tmp = bunsan::tempfile::in_dir(destination);
		bunsan::reset_dir(tmp->path());
		extractor(source, tmp->path());
		move_all_from(tmp->path()/subsource, destination);
	}
}

namespace
{
	bool has_child(const boost::property_tree::ptree &ptree, const boost::property_tree::ptree::path_type &path)
	{
		return ptree.get_child_optional(path);
	}
}

void bunsan::pm::repository::native::unpack_import(const entry &package, const boost::filesystem::path &destination,
	std::map<std::string, boost::property_tree::ptree> &snapshot)
{
	SLOG("starting "<<package<<" import unpack");
	bunsan::executor extractor(config.get_child(command_unpack));
	boost::property_tree::ptree index;
	read_index(package, index);
	if (snapshot.find(package.name())==snapshot.end())
		read_checksum(package, snapshot[package.name()]);
	for (const auto &i: index.get_child(child_sources))
		::extract(extractor, source_resource(package, i.second.get_value<std::string>()+value(suffix_src)),
			destination/i.first, i.second.get_value<std::string>());
	for (const auto &i: index.get_child(child_imports))
		unpack_import(i.second.get_value<std::string>(), destination/i.first, snapshot);
}

void bunsan::pm::repository::native::unpack(const entry &package, const boost::filesystem::path &build_dir)
{
	try
	{
		SLOG("starting "<<package<<" "<<__func__);
		boost::filesystem::path src = build_dir/value(name_dir_source);
		boost::filesystem::path snp = build_dir/value(name_file_snapshot);
		bunsan::reset_dir(src);
		boost::property_tree::ptree snapshot, snp_imports;
		std::map<std::string, boost::property_tree::ptree> snp_imports_map;
		unpack_import(package, src, snp_imports_map);
		for (const auto &i: snp_imports_map)
			snp_imports.push_back(boost::property_tree::ptree::value_type(i.first, i.second));
		snapshot.put_child(child_imports, snp_imports);
		boost::property_tree::write_info(snp.native(), snapshot);
	}
	catch (std::exception &e)
	{
		throw pm_error("Unable to unpack package", e);
	}
}

void bunsan::pm::repository::native::configure(const entry &package, const boost::filesystem::path &build_dir)
{
	try
	{
		SLOG("starting "<<package<<" "<<__func__);
		boost::filesystem::path build = build_dir/value(name_dir_build);
		boost::filesystem::path deps = build_dir/value(name_dir_depends);
		boost::filesystem::path snp = build_dir/value(name_file_snapshot);
		boost::property_tree::ptree snapshot, snp_depends;
		std::map<std::string, boost::property_tree::ptree> snp_depends_map;
		boost::property_tree::read_info(snp.native(), snapshot);
		bunsan::reset_dir(build);
		bunsan::reset_dir(deps);
		bunsan::executor exec(config.get_child(command_configure));
		exec.current_path(build).add_argument(build_dir/value(name_dir_source));
		for (const auto &i: depends(package))
		{
			bunsan::tempfile_ptr dep = bunsan::tempfile::in_dir(deps);
			extract(i.second, dep->path());
			exec.add_argument("-D"+i.first+"="+dep->native());
			if (snp_depends_map.find(i.first)==snp_depends_map.end())
			{
				boost::property_tree::ptree snapshot_;
				boost::property_tree::read_info(package_resource(i.second, value(name_file_snapshot)).native(), snapshot_);
				snp_depends_map[i.first] = snapshot_.get_child(child_imports);
			}
			dep->auto_remove(false);
		}
		for (const auto &i: snp_depends_map)
			snp_depends.push_back(boost::property_tree::ptree::value_type(i.first, i.second));
		snapshot.put_child(child_depends, snp_depends);
		boost::property_tree::write_info(snp.native(), snapshot);
		exec();
	}
	catch (std::exception &e)
	{
		throw pm_error("Unable to configure package", e);
	}
	
}

void bunsan::pm::repository::native::compile(const entry &package, const boost::filesystem::path &build_dir)
{
	try
	{
		SLOG("starting "<<package<<" "<<__func__);
		boost::filesystem::path build = build_dir/value(name_dir_build);
		bunsan::executor::exec_from(build, config.get_child(command_compile));
	}
	catch (std::exception &e)
	{
		throw pm_error("Unable to compile package", e);
	}
}

void bunsan::pm::repository::native::pack(const entry &package, const boost::filesystem::path &build_dir)
{
	try
	{
		SLOG("starting "<<package<<" "<<__func__);
		boost::filesystem::path build = build_dir/value(name_dir_build);
		boost::filesystem::path snp = build_dir/value(name_file_snapshot);
		bunsan::executor::exec_from(build, config.get_child(command_pack));
		boost::filesystem::create_directories(package.local_resource(value(dir_package)));
		boost::filesystem::copy_file(build/value(name_file_pkg), package_resource(package, value(name_file_pkg)),
			boost::filesystem::copy_option::overwrite_if_exists);
		boost::filesystem::copy_file(snp, package_resource(package, value(name_file_snapshot)),
			boost::filesystem::copy_option::overwrite_if_exists);
	}
	catch (std::exception &e)
	{
		throw pm_error("Unable to pack package", e);
	}
}

std::map<std::string, bunsan::pm::entry> bunsan::pm::repository::native::depends(const entry &package)
{
	try
	{
		SLOG("trying to get depends for "<<package);
		boost::property_tree::ptree index;
		read_index(package, index);
		std::map<std::string, entry> map;
		for (const auto &i: index.get_child(child_depends))
		{
			bunsan::pm::entry e(i.second.get_value<std::string>());
			if (map.find(i.first)!=map.end())
				throw std::runtime_error("dublicate dependencies: \""+i.first+"\"");
			map.insert(std::map<std::string, entry>::value_type(i.first, e));
		}
		SLOG("found \""<<map.size()<<"\" dependencies");
		return map;
	}
	catch (std::exception &e)
	{
		throw pm_error("Unable to read package depends", e);
	}
}

std::multimap<boost::filesystem::path, bunsan::pm::entry> bunsan::pm::repository::native::imports(const entry &package)
{
	try
	{
		SLOG("trying to get imports for "<<package);
		boost::property_tree::ptree index;
		read_index(package, index);
		std::multimap<boost::filesystem::path, entry> map;
		for (const auto &i: index.get_child(child_imports))
		{
			bunsan::pm::entry e(i.second.get_value<std::string>());
			map.insert(std::multimap<std::string, entry>::value_type(i.first, e));
		}
		SLOG("found \""<<map.size()<<"\" imports");
		return map;
	}
	catch (std::exception &e)
	{
		throw pm_error("Unable to read package imports", e);
	}
}

bool bunsan::pm::repository::native::package_outdated(const entry &package)
{
	if (!boost::filesystem::exists(package_resource(package, value(name_file_pkg))))
		return true;
	boost::property_tree::ptree index;
	read_index(package, index);
	std::function<void(const entry &, std::map<std::string, boost::property_tree::ptree> &)> build_imports_map =
		[this, &build_imports_map](const entry &package, std::map<std::string, boost::property_tree::ptree> &map)
		{
			if (map.find(package.name())==map.end())
			{
				read_checksum(package, map[package.name()]);
				for (const auto &i: imports(package))
					build_imports_map(i.second, map);
			}
		};
	auto equal_imports = [this, &build_imports_map](const entry &package)->bool
		{
			boost::filesystem::path snp = package_resource(package, value(name_file_snapshot));
			if (!boost::filesystem::exists(snp))
				return false;
			boost::property_tree::ptree snapshot;
			boost::property_tree::read_info(snp.native(), snapshot);
			boost::property_tree::ptree &snp_imports = snapshot.get_child(child_imports);
			std::map<std::string, boost::property_tree::ptree> current_imports_map, built_imports_map;
			build_imports_map(package, current_imports_map);
			for (const auto &i: snp_imports)
				built_imports_map[i.first] = i.second;
			return current_imports_map==built_imports_map;
		};
	// imports
	if (!equal_imports(package))
		return true;
	// depends
	for (const auto &i: depends(package))// depend set and snp_depends are the same because package has the same index as before
		if (!equal_imports(i.first))
			return true;
	return false;
}

void bunsan::pm::repository::native::extract(const entry &package, const boost::filesystem::path &destination)
{
	try
	{
		SLOG("starting "<<package<<" "<<__func__);
		bunsan::executor extractor(config.get_child(command_extract));
		bunsan::reset_dir(destination);
		::extract(extractor, package_resource(package, value(name_file_pkg)), destination, value(name_dir_pkg));
	}
	catch (std::exception &e)
	{
		throw pm_error("Unable to extract package", e);
	}
}

