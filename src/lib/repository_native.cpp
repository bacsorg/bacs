#include "repository_native.hpp"
#include "repository_config.hpp"

#include <stdexcept>

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/info_parser.hpp>
#include <boost/format.hpp>
#include <boost/filesystem.hpp>
#include <boost/filesystem/fstream.hpp>

#include "bunsan/compatibility/boost.hpp"

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
		if (!boost::filesystem::is_directory(output))
			boost::filesystem::create_directories(output);
		bunsan::compatibility::boost::filesystem::copy_file(checksum_ptr->path(), output/value(name_file_checksum));
		boost::property_tree::ptree checksum;
		read_checksum(package, checksum);
		if (outdated(output/value(name_file_index), checksum.get<std::string>(name_file_index)))
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

void bunsan::pm::repository::native::unpack_import(const entry &package, const boost::filesystem::path &destination)
{
	SLOG("starting "<<package<<" import unpack");
	bunsan::executor extractor(config.get_child(command_unpack));
	boost::property_tree::ptree index;
	read_index(package, index);
	for (const auto &i: index.get_child(child_sources))
	{
		::extract(extractor, local_resource(package, i.second.get_value<std::string>()+value(suffix_src)), destination/i.first);
	}
	for (const auto &i: index.get_child(child_imports))
	{
		unpack_import(i.second.get_value<std::string>(), destination/i.first);
	}
}

void bunsan::pm::repository::native::unpack(const entry &package, const boost::filesystem::path &build_dir)
{
	try
	{
		SLOG("starting "<<package<<" "<<__func__);
		boost::filesystem::path src = build_dir/value(name_dir_source);
		bunsan::reset_dir(src);
		unpack_import(package, src);
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
		bunsan::reset_dir(build);
		bunsan::reset_dir(deps);
		bunsan::executor exec(config.get_child(command_configure));
		exec.current_path(build).add_argument(build_dir/value(name_dir_source));
		for (const auto &i: depends(package))
		{
			bunsan::tempfile_ptr dep = bunsan::tempfile::in_dir(deps);
			extract(i.second, dep->path());
			exec.add_argument("-D"+i.first+"="+dep->native());
			dep->auto_remove(false);
		}
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

#if 0
void bunsan::pm::repository::native::pack(const entry &package, const boost::filesystem::path &build_dir)
{
	try
	{
		SLOG("starting "<<package<<" "<<__func__);
		boost::filesystem::path build = build_dir;
		build = build/value("name.dir.build");
		boost::filesystem::path pkgdir = value("dir.package");
		pkgdir /= package;
		bunsan::reset_dir(pkgdir);
		boost::filesystem::path srcdir = value("dir.source");
		srcdir /= package;
		bunsan::executor::exec_from(build, config.get_child("command.pack"));
		bunsan::compatibility::boost::filesystem::copy_file
			(build/value("name.file.pkg"), pkgdir/value("name.file.pkg"));
		bunsan::compatibility::boost::filesystem::copy_file
			(srcdir/value("name.file.time"), pkgdir/value("name.file.time"));
		// we should copy all depends times
		boost::filesystem::path depends_time = pkgdir/value("name.dir.depends_time");
		bunsan::reset_dir(depends_time);
		boost::filesystem::path packages = value("dir.source");
		for (const auto &i: depends(package))
			bunsan::compatibility::boost::filesystem::copy_file
				(packages/i/value("name.file.time"), depends_time/i);
	}
	catch (std::exception &e)
	{
		throw pm_error("Unable to pack package", e);
	}
}
#endif

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

#if 0
bool bunsan::pm::repository::native::package_outdated(const entry &package)
{
	bunsan::pm::repository::check_package_name(package);
	boost::filesystem::path src = value("dir.source");
	boost::filesystem::path sources = src;
	boost::filesystem::path pkg = value("dir.package");
	src /= package;
	pkg /= package;
	bool outdated = false;
	outdated = outdated || !boost::filesystem::exists(pkg);
	outdated = outdated || !boost::filesystem::exists(pkg/value("name.file.time"));
	outdated = outdated || !boost::filesystem::exists(pkg/value("name.file.pkg"));
	outdated = outdated || !boost::filesystem::exists(pkg/value("name.dir.depends_time"));
	outdated = outdated || !equal_files(src/value("name.file.time"), pkg/value("name.file.time"));
	if (!outdated)
		for (const auto &i: depends(package))
		{
			outdated = outdated ||
				!equal_files(sources/i/value("name.file.time"), pkg/value("name.dir.depends_time")/i);
			if (outdated)
				break;
		}
	return outdated;
}
#endif

void bunsan::pm::repository::native::extract(const entry &package, const boost::filesystem::path &destination)
{
	try
	{
		SLOG("starting "<<package<<" "<<__func__);
		bunsan::executor extractor(config.get_child(command_extract));
		bunsan::reset_dir(destination);
		::extract(extractor, local_resource(package, value(name_file_pkg)), destination, value(name_dir_pkg));
	}
	catch (std::exception &e)
	{
		throw pm_error("Unable to extract package", e);
	}
}

