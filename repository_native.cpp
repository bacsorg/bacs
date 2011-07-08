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

bunsan::pm::repository::native::native(const boost::property_tree::ptree &config_): config(config_){}

void bunsan::pm::repository::native::build(const std::string &package)
{
	bunsan::pm::repository::check_package_name(package);
	bunsan::tempfile_ptr build_dir = bunsan::tempfile::in_dir(config.get<std::string>("dir.tmp"));
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

	bool equal_files(const boost::filesystem::path &file1, const boost::filesystem::path &file2)
	{
		// TODO: not checked
		boost::filesystem::ifstream fin1(file1, std::ios_base::in | std::ios_base::binary), fin2(file2, std::ios_base::in | std::ios_base::binary);
		if (!fin1.good())
			throw std::runtime_error("bad file \""+file1.native()+"\"");
		if (!fin2.good())
			throw std::runtime_error("bad file \""+file2.native()+"\"");
		char c1, c2;
		while (fin1.get(c1) && fin2.get(c2) && c1==c2);
		bool eq = c1==c2;
		eq = eq && (!fin1.get(c1)) && (!fin2.get(c2));
		SLOG("files "<<file1<<" and "<<file2<<" are "<<(eq?"equal":"not equal"));
		return eq;
	}

	void package_path(const boost::property_tree::ptree &config, const std::string &package, std::string &url, boost::filesystem::path &output)
	{
		url = config.get<std::string>("repository")+"/"+package+"/";
		output = config.get<std::string>("dir.source");
		output /= package;
	}
}

bool bunsan::pm::repository::native::update_meta(const std::string &package)
{
	bunsan::pm::repository::check_package_name(package);
	try
	{
		SLOG("starting \""<<package<<"\" "<<__func__);
		bunsan::executor fetcher(config.get_child("command.fetch"));
		std::string url;
		boost::filesystem::path output;
		package_path(config, package, url, output);
		bool is_dir = boost::filesystem::is_directory(output);
		bunsan::tempfile output_tmp(output, !is_dir);// guard to remove output dir if something fails
		if (!is_dir)
			bunsan::reset_dir(output);
		bunsan::tempfile_ptr time_tmp = bunsan::tempfile::from_model(config.get<std::string>("name.file.tmp"));
		std::string name_file_time = config.get<std::string>("name.file.time");
		std::string name_file_depends = config.get<std::string>("name.file.depends");
		fetcher(url+name_file_time, time_tmp->path().native());
		bool updated = false;
		if (	!boost::filesystem::exists(output/name_file_time) ||
			!boost::filesystem::exists(output/name_file_depends) ||
			!equal_files(output/name_file_time, time_tmp->path()) )
		{
			output_tmp.auto_remove(true);
			bunsan::reset_dir(output);
			bunsan::compatibility::boost::filesystem::copy_file(time_tmp->path(), output/name_file_time);
			fetcher(url+name_file_depends, (output/name_file_depends).native());
			updated = true;
		}
		output_tmp.auto_remove(false);// everything is ok, so we don't need to remove output dir
		return updated;
	}
	catch (std::exception &e)
	{
		throw pm_error("Unable to update package meta info", e);
	}
}

void bunsan::pm::repository::native::fetch_source(const std::string &package)
{
	bunsan::pm::repository::check_package_name(package);
	try
	{
		SLOG("starting \""<<package<<"\" "<<__func__);
		bunsan::executor fetcher(config.get_child("command.fetch"));
		std::string url;
		boost::filesystem::path output;
		package_path(config, package, url, output);
		bool is_dir = boost::filesystem::is_directory(output);
		if (!is_dir)
			throw std::runtime_error("source directory \""+output.native()+"\" does not exists");
		std::string name_file_src = config.get<std::string>("name.file.src");
		fetcher(url+name_file_src, (output/name_file_src).native());
	}
	catch (std::exception &e)
	{
		throw pm_error("Unable to fetch package source", e);
	}
}

void bunsan::pm::repository::native::unpack(const std::string &package, const boost::filesystem::path &build_dir)
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

void bunsan::pm::repository::native::configure(const std::string &package, const boost::filesystem::path &build_dir)
{
	try
	{
		SLOG("starting \""<<package<<"\" "<<__func__);
		boost::filesystem::path build = build_dir;
		build = build/config.get<std::string>("name.dir.build");
		bunsan::reset_dir(build);
		bunsan::executor exec(config.get_child("command.configure"));
		exec.current_path(build).add_argument((build.parent_path()/config.get<std::string>("name.dir.src")).native());
		std::map<std::string, std::string> deps = depend_keys(package);
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

void bunsan::pm::repository::native::compile(const std::string &package, const boost::filesystem::path &build_dir)
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

void bunsan::pm::repository::native::pack(const std::string &package, const boost::filesystem::path &build_dir)
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

std::vector<std::string> bunsan::pm::repository::native::depends(const std::string &package)
{
	auto map = depend_keys(package);
	std::vector<std::string> deps(map.size());
	deps.resize(0);
	for (const auto &i: map)
		deps.push_back(i.second);
	return deps;
}

std::map<std::string, std::string> bunsan::pm::repository::native::depend_keys(const std::string &package)
{
	try
	{
		SLOG("trying to get depends for \""<<package<<"\"");
		boost::property_tree::ptree deps;
		boost::filesystem::path deps_file = config.get<std::string>("dir.source");
		deps_file /= package;
		deps_file /= config.get<std::string>("name.file.depends");
		boost::property_tree::read_info(deps_file.native(), deps);
		std::map<std::string, std::string> map;
		for (const auto &i: deps)
		{
			bunsan::pm::repository::check_package_name(i.second.get_value<std::string>());
			if (map.find(i.first)!=map.end())
				throw std::runtime_error("dublicate dependencies: \""+i.first+"\"");
			map[i.first] = i.second.get_value<std::string>();
		}
		SLOG("found \""<<map.size()<<"\" dependencies");
		return map;
	}
	catch (std::exception &e)
	{
		throw pm_error("Unable to fetch package depends", e);
	}
}

bool bunsan::pm::repository::native::source_outdated(const std::string &package)
{
	bunsan::pm::repository::check_package_name(package);
	bunsan::tempfile_ptr tmp = bunsan::tempfile::from_model(config.get<std::string>("name.file.tmp"));
	boost::filesystem::path src = config.get<std::string>("dir.source");
	src = src/package;
	bool outdated = false;
	outdated = outdated || !boost::filesystem::exists(src);
	outdated = outdated || !boost::filesystem::exists(src/config.get<std::string>("name.file.time"));
	outdated = outdated || !boost::filesystem::exists(src/config.get<std::string>("name.file.depends"));
	if (outdated)
		throw pm_error("Unable to check package outdateness", std::runtime_error("You have to update meta info before any checks"));//XXX check message
	outdated = outdated || !boost::filesystem::exists(src/config.get<std::string>("name.file.src"));
	return outdated;
}

bool bunsan::pm::repository::native::package_outdated(const std::string &package)
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

void bunsan::pm::repository::native::extract(const std::string &package, const boost::filesystem::path &destination)
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

