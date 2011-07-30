#include "repository_native.hpp"

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

#include "bunsan/pm/hash.hpp"

namespace
{
	constexpr const char *child_depends = "depends";
	constexpr const char *child_sources = "sources";
	constexpr const char *child_imports = "imports";

	constexpr const char *dir_tmp = "dir.tmp";
	constexpr const char *dir_package = "dir.package";
	constexpr const char *dir_source = "dir.source";

	constexpr const char *name_file_tmp = "name.file.tmp";
	constexpr const char *name_file_index = "name.file.index";
	constexpr const char *name_file_hash = "name.file.hash";

	constexpr const char *name_dir_pkg = "name.dir.pkg";
	constexpr const char *name_dir_build = "name.dir.build";

	constexpr const char *suffix_src = "suffix.src";
	constexpr const char *suffix_pkg = "suffix.pkg";

	constexpr const char *command_fetch = "command.fetch";
	constexpr const char *command_unpack = "command.unpack";
	constexpr const char *command_configure = "command.configure";
	constexpr const char *command_compile = "command.compile";
	constexpr const char *command_pack = "command.pack";
	constexpr const char *command_extract = "command.extract";

	constexpr const char *repository_url = "repository";
}

bunsan::pm::repository::native::native(const boost::property_tree::ptree &config_): config(config_){}

void bunsan::pm::repository::native::build(const entry &package)
{
	bunsan::tempfile_ptr build_dir = bunsan::tempfile::in_dir(config.get<std::string>(dir_tmp));
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
		return !boost::filesystem::exists(file) || bunsan::pm::hash(file)!=checksum;
	}
}

void bunsan::pm::repository::native::update_index(const entry &package)
{
	try
	{
		SLOG("starting \""<<package<<"\" "<<__func__);
		bunsan::executor fetcher(config.get_child(command_fetch));
		bunsan::tempfile_ptr hash_ptr = bunsan::tempfile::from_model(config.get<std::string>(name_file_tmp));
		try
		{
			fetcher(package.remote_resource(config.get<std::string>(repository_url), config.get<std::string>(name_file_hash)), hash_ptr->path());
		}
		catch (std::exception &e)
		{
			throw pm_error("Unable to download package meta info (no such package in repository)", e);
		}
		boost::filesystem::path output = package.local_resource(config.get<std::string>(dir_source));
		if (!boost::filesystem::is_directory(output))
			boost::filesystem::create_directories(output);
		bunsan::compatibility::boost::filesystem::copy_file(hash_ptr->path(), output/config.get<std::string>(name_file_hash));
		boost::property_tree::ptree hash;
		boost::property_tree::read_info((output/config.get<std::string>(name_file_hash)).native(), hash);
		if (outdated(output/config.get<std::string>(name_file_index), hash.get<std::string>(name_file_index)))
			fetcher(package.remote_resource(config.get<std::string>(repository_url), config.get<std::string>(name_file_index)),
				output/config.get<std::string>(name_file_index));
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
		SLOG("starting \""<<package<<"\" "<<__func__);
		const std::string src_sfx = config.get<std::string>(suffix_src);
		bunsan::executor fetcher(config.get_child(command_fetch));
		boost::filesystem::path output = package.remote_resource(config.get<std::string>(dir_source));
		boost::property_tree::ptree index, hash;
		boost::property_tree::read_info((output/config.get<std::string>(name_file_index)).native(), index);
		boost::property_tree::read_info((output/config.get<std::string>(name_file_hash)).native(), hash);
		for (const auto &i: index.get_child(child_sources))
		{
			std::string src = i.second.get_value<std::string>();
			if (outdated(output/(src+src_sfx), hash.get<std::string>(src)))
				fetcher(package.remote_resource(config.get<std::string>(repository_url)), output/(src+src_sfx));
		}
	}
	catch (std::exception &e)
	{
		throw pm_error("Unable to fetch package source", e);
	}
}

#if 0
void bunsan::pm::repository::native::unpack(const entry &package, const boost::filesystem::path &build_dir)
{
	try
	{
		SLOG("starting \""<<package<<"\" "<<__func__);
		boost::filesystem::path build = build_dir;
		bunsan::reset_dir(build);
		bunsan::executor::exec_from(
			build,
			config.get_child("command.unpack"),
			(boost::filesystem::path(config.get<std::string>("dir.source"))/package/config.get<std::string>("name.file.src")).native(),
			build.native());
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
		SLOG("starting \""<<package<<"\" "<<__func__);
		boost::filesystem::path build = build_dir;
		build = build/config.get<std::string>("name.dir.build");
		bunsan::reset_dir(build);
		bunsan::executor exec(config.get_child("command.configure"));
		exec.current_path(build).add_argument((build.parent_path()/config.get<std::string>("name.dir.src")).native());
		std::map<std::string, entry> deps = depend_keys(package);
		for (const auto &i: deps)
		{
			bunsan::reset_dir(build/(i.second));
			extract(i.second, build/i.second);
			exec.add_argument("-D"+i.first+"="+(build/i.second).native());
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
		SLOG("starting \""<<package<<"\" "<<__func__);
		boost::filesystem::path build = build_dir;
		build = build/config.get<std::string>("name.dir.build");
		bunsan::executor::exec_from(build, config.get_child("command.compile"));
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
		SLOG("starting \""<<package<<"\" "<<__func__);
		boost::filesystem::path build = build_dir;
		build = build/config.get<std::string>("name.dir.build");
		boost::filesystem::path pkgdir = config.get<std::string>("dir.package");
		pkgdir /= package;
		bunsan::reset_dir(pkgdir);
		boost::filesystem::path srcdir = config.get<std::string>("dir.source");
		srcdir /= package;
		bunsan::executor::exec_from(build, config.get_child("command.pack"));
		bunsan::compatibility::boost::filesystem::copy_file
			(build/config.get<std::string>("name.file.pkg"), pkgdir/config.get<std::string>("name.file.pkg"));
		bunsan::compatibility::boost::filesystem::copy_file
			(srcdir/config.get<std::string>("name.file.time"), pkgdir/config.get<std::string>("name.file.time"));
		// we should copy all depends times
		boost::filesystem::path depends_time = pkgdir/config.get<std::string>("name.dir.depends_time");
		bunsan::reset_dir(depends_time);
		boost::filesystem::path packages = config.get<std::string>("dir.source");
		for (const auto &i: depends(package))
			bunsan::compatibility::boost::filesystem::copy_file
				(packages/i/config.get<std::string>("name.file.time"), depends_time/i);
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
		SLOG("trying to get depends for \""<<package<<"\"");
		boost::property_tree::ptree index;
		boost::property_tree::read_info(package.local_resource(config.get<std::string>(dir_source),
			config.get<std::string>(name_file_index)).native(), index);
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
		SLOG("trying to get imports for \""<<package<<"\"");
		boost::property_tree::ptree index;
		boost::property_tree::read_info(package.local_resource(config.get<std::string>(dir_source),
			config.get<std::string>(name_file_index)).native(), index);
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
	boost::filesystem::path src = config.get<std::string>("dir.source");
	boost::filesystem::path sources = src;
	boost::filesystem::path pkg = config.get<std::string>("dir.package");
	src /= package;
	pkg /= package;
	bool outdated = false;
	outdated = outdated || !boost::filesystem::exists(pkg);
	outdated = outdated || !boost::filesystem::exists(pkg/config.get<std::string>("name.file.time"));
	outdated = outdated || !boost::filesystem::exists(pkg/config.get<std::string>("name.file.pkg"));
	outdated = outdated || !boost::filesystem::exists(pkg/config.get<std::string>("name.dir.depends_time"));
	outdated = outdated || !equal_files(src/config.get<std::string>("name.file.time"), pkg/config.get<std::string>("name.file.time"));
	if (!outdated)
		for (const auto &i: depends(package))
		{
			outdated = outdated ||
				!equal_files(sources/i/config.get<std::string>("name.file.time"), pkg/config.get<std::string>("name.dir.depends_time")/i);
			if (outdated)
				break;
		}
	return outdated;
}

void bunsan::pm::repository::native::extract(const entry &package, const boost::filesystem::path &destination)
{
	try
	{
		bunsan::pm::repository::check_package_name(package);
		SLOG("starting \""<<package<<"\" "<<__func__);
		bunsan::reset_dir(destination);
		bunsan::tempfile_ptr tmp = bunsan::tempfile::in_dir(destination);
		boost::filesystem::path pkgdir = config.get<std::string>("dir.package");
		pkgdir /= package;
		bunsan::reset_dir(tmp->path());
		bunsan::executor::exec(config.get_child("command.extract"), (pkgdir/config.get<std::string>("name.file.pkg")).native(), tmp->native());
		for (auto i = boost::filesystem::directory_iterator(tmp->path()/config.get<std::string>("name.dir.pkg")); i!=boost::filesystem::directory_iterator(); ++i)
		{
			SLOG("moving "<<i->path()<<" to "<<destination/(i->path().filename()));
			boost::filesystem::rename(i->path(), destination/(i->path().filename()));
		}
	}
	catch (std::exception &e)
	{
		throw pm_error("Unable to extract package", e);
	}
}
#endif

