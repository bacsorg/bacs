#include "bunsan/config.hpp"

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
	bool outdated(const boost::filesystem::path &file, const std::string &checksum)
	{
		return !boost::filesystem::exists(file) || bunsan::pm::checksum(file)!=checksum;
	}
	void load(const bunsan::executor &fetcher, const std::string &source, const boost::filesystem::path &file, const std::string &checksum)
	{
		if (outdated(file, checksum))
		{
			fetcher(source, file);
			if (outdated(file, checksum))
				throw std::runtime_error("Error loading file \""+file.native()+"\": wrong checksum");
		}
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
		load(fetcher,
			remote_resource(package, value(name_file_index)),
			output/value(name_file_index),
			checksum.get<std::string>(value(name_file_index)));
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
		boost::property_tree::ptree checksum;
		read_checksum(package, checksum);
		for (const auto &i: sources(package))
			load(fetcher,
				remote_resource(package, i.second+src_sfx),
				output/(i.second+src_sfx),
				checksum.get<std::string>(i.second));
	}
	catch (std::exception &e)
	{
		throw pm_error("Unable to fetch package source", e);
	}
}

namespace
{
	void merge_dir(const boost::filesystem::path &source, const boost::filesystem::path &destination)
	{
		SLOG("merging dirs: source = "<<source<<", destination = "<<destination);
		for (auto i = boost::filesystem::directory_iterator(source); i!=boost::filesystem::directory_iterator(); ++i)
		{
			boost::filesystem::path src = i->path(), dst = destination/i->path().filename();
			if (boost::filesystem::is_directory(src) && boost::filesystem::is_directory(dst))
				merge_dir(src, dst);
			else
				boost::filesystem::rename(src, dst);
		}
	}
	void extract(const bunsan::executor &extractor, const boost::filesystem::path &source,
		const boost::filesystem::path &destination, const boost::filesystem::path &subsource=boost::filesystem::path())
	{
		boost::filesystem::create_directories(destination);
		bunsan::tempfile_ptr tmp = bunsan::tempfile::in_dir(destination);
		bunsan::reset_dir(tmp->path());
		extractor(source, tmp->path());
		merge_dir(tmp->path()/subsource, destination);
	}
}

void bunsan::pm::repository::native::unpack_import(const entry &package, const boost::filesystem::path &destination,
	std::map<std::string, boost::property_tree::ptree> &snapshot)
{
	SLOG("starting "<<package<<" import unpack");
	bunsan::executor extractor(config.get_child(command_unpack));
	boost::property_tree::ptree index;
	// extract sources
	for (const auto &i: sources(package))
		::extract(extractor, source_resource(package, i.second+value(suffix_src)), destination/i.first, i.second);
	if (snapshot.find(package.name())==snapshot.end())
		read_checksum(package, snapshot[package.name()]);
	// extract source imports
	for (const auto &i: source_imports(package))
		unpack_import(i.second, destination/i.first, snapshot);
	// extract package imports
	for (const auto &i: package_imports(package))
	{
		SLOG("starting "<<package<<" import extract");
		boost::filesystem::path snp = package_resource(i.second, value(name_file_snapshot));
		bunsan::executor extractor(config.get_child(command_extract));
		::extract(extractor, package_resource(i.second, value(name_file_pkg)), destination/i.first, value(name_dir_pkg));
		boost::property_tree::ptree snapshot_;
		boost::property_tree::read_info(snp.native(), snapshot_);
		for (const auto &j: snapshot_)// \todo is this check useful? if yes then we should perform same check in code above
		{
			auto it = snapshot.find(j.first);
			if (it==snapshot.end())
				snapshot[j.first] = j.second;
			else if (it->second!=j.second)
				throw std::runtime_error("Different package versions of \""+j.first+"\" in \""+i.second.name()+"\" extract");
		}
	}
}

void bunsan::pm::repository::native::unpack(const entry &package, const boost::filesystem::path &build_dir)
{
	try
	{
		SLOG("starting "<<package<<" "<<__func__);
		boost::filesystem::path src = build_dir/value(name_dir_source);
		boost::filesystem::path snp = build_dir/value(name_file_snapshot);
		bunsan::reset_dir(src);
		boost::property_tree::ptree snapshot;
		std::map<std::string, boost::property_tree::ptree> snapshot_map;
		unpack_import(package, src, snapshot_map);
		for (const auto &i: snapshot_map)
			snapshot.push_back(boost::property_tree::ptree::value_type(i.first, i.second));
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
		bunsan::reset_dir(build);
		bunsan::executor::exec_from(build, config.get_child(command_configure), build_dir/value(name_dir_source));
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

bool bunsan::pm::repository::native::package_outdated(const entry &package)
{
	if (!boost::filesystem::exists(package_resource(package, value(name_file_pkg))))
	{
		SLOG("\""<<value(name_file_pkg)<<"\" was not found for "<<package<<" => outdated");
		return true;
	}
	boost::filesystem::path snp = package_resource(package, value(name_file_snapshot));
	if (!boost::filesystem::exists(snp))
	{
		SLOG(snp.filename()<<" was not found for "<<package<<" => outdated");
		return true;
	}
	std::map<std::string, boost::property_tree::ptree> snapshot_map, snapshot_map_;
	std::function<void(const entry &, std::map<std::string, boost::property_tree::ptree> &)> build_imports_map =
		[this, &build_imports_map](const entry &package, std::map<std::string, boost::property_tree::ptree> &map)
		{
			if (map.find(package.name())==map.end())
			{
				read_checksum(package, map[package.name()]);
				for (const auto &i: source_imports(package))
					build_imports_map(i.second, map);
				for (const auto &i: package_imports(package))
					build_imports_map(i.second, map);
			}
		};
	build_imports_map(package, snapshot_map_);
	// reads snapshot_map from built package
	{
		boost::property_tree::ptree snapshot;
		boost::property_tree::read_info(snp.native(), snapshot);
		for (const auto &i: snapshot)
			snapshot_map[i.first] = i.second;
	}
	return snapshot_map!=snapshot_map_;
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

