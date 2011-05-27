#include "repository.hpp"
#include "executor.hpp"

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/info_parser.hpp>
#include <boost/format.hpp>
#include <boost/filesystem.hpp>
#include <boost/filesystem/fstream.hpp>

#include "util.hpp"
#include "tempfile.hpp"

void reset_dir(const boost::filesystem::path &dir)
{
	SLOG("trying to reset "<<dir);
	if (boost::filesystem::exists(dir))
		boost::filesystem::remove_all(dir);
	boost::filesystem::create_directory(dir);
	SLOG("directory "<<dir<<" was reset");
}

its::repository::native::native(const boost::property_tree::ptree *config_): config(config_){}

void its::repository::native::build(const std::string &package)
{
	its::tempfile build_dir(boost::filesystem::path(config->get<std::string>("dir.tmp"))/"%%%%-%%%%-%%%%-%%%%");
	reset_dir(build_dir.path());
	unpack(package, build_dir.path());
	configure(package, build_dir.path());
	compile(package, build_dir.path());
	pack(package, build_dir.path());
}

void its::repository::native::fetch(const std::string &package)
{
	SLOG("starting \""<<package<<"\" "<<__func__);
	its::executor fetcher(config->get_child("command.fetch"));
	std::string url = config->get<std::string>("repository")+"/"+package+"/";
	boost::filesystem::path output = config->get<std::string>("dir.source");
	output /= package;
	if (boost::filesystem::exists(output))
		boost::filesystem::remove_all(output);
	boost::filesystem::create_directory(output);
	fetcher(url+config->get<std::string>("name.file.src"), (output/config->get<std::string>("name.file.src")).native());
	fetcher(url+config->get<std::string>("name.file.time"), (output/config->get<std::string>("name.file.time")).native());
	fetcher(url+config->get<std::string>("name.file.depends"), (output/config->get<std::string>("name.file.depends")).native());
}

void its::repository::native::unpack(const std::string &package, const boost::filesystem::path &build_dir)
{
	SLOG("starting \""<<package<<"\" "<<__func__);
	boost::filesystem::path build = build_dir;
	reset_dir(build);
	its::executor::exec_from(
		build,
		config->get_child("command.unpack"),
		(boost::filesystem::path(config->get<std::string>("dir.source"))/package/config->get<std::string>("name.file.src")).native(),
		build.native());
}

void its::repository::native::configure(const std::string &package, const boost::filesystem::path &build_dir)
{
	SLOG("starting \""<<package<<"\" "<<__func__);
	boost::filesystem::path build = build_dir;
	build = build/config->get<std::string>("name.dir.build");
	reset_dir(build);
	its::executor exec(config->get_child("command.configure"));
	exec.current_path(build).add_argument((build.parent_path()/config->get<std::string>("name.dir.src")).native());
	std::map<std::string, std::string> deps = depend_keys(package);
	for (auto i = deps.cbegin(); i!=deps.cend(); ++i)
	{
		reset_dir(build/(i->second));
		extract(i->second, build/i->second);
		exec.add_argument("-DDEPEND_"+i->first+"="+(build/i->second).native());
	}
	exec();
}

void its::repository::native::compile(const std::string &package, const boost::filesystem::path &build_dir)
{
	SLOG("starting \""<<package<<"\" "<<__func__);
	boost::filesystem::path build = build_dir;
	build = build/config->get<std::string>("name.dir.build");
	executor::exec_from(build, config->get_child("command.compile"));
}

void its::repository::native::pack(const std::string &package, const boost::filesystem::path &build_dir)
{
	SLOG("starting \""<<package<<"\" "<<__func__);
	boost::filesystem::path build = build_dir;
	build = build/config->get<std::string>("name.dir.build");
	boost::filesystem::path pkgdir = config->get<std::string>("dir.package");
	pkgdir /= package;
	reset_dir(pkgdir);
	boost::filesystem::path srcdir = config->get<std::string>("dir.source");
	srcdir /= package;
	executor::exec_from(build, config->get_child("command.pack"));
	boost::filesystem3::copy(build/config->get<std::string>("name.file.pkg"), pkgdir/config->get<std::string>("name.file.pkg"));
	boost::filesystem3::copy(srcdir/config->get<std::string>("name.file.time"), pkgdir/config->get<std::string>("name.file.time"));
}

std::vector<std::string> its::repository::native::depends(const std::string &package)
{
	auto map = depend_keys(package);
	std::vector<std::string> deps(map.size());
	deps.resize(0);
	for (auto i = map.cbegin(); i!=map.cend(); ++i)
		deps.push_back(i->second);
	return deps;
}

std::map<std::string, std::string> its::repository::native::depend_keys(const std::string &package)
{
	SLOG("trying to get depends for \""<<package<<"\"");
	its::tempfile dfile(config->get<std::string>("name.file.tmp"));
	SLOG("\""<<package<<"\" depends will be downloaded to "<<dfile.path());
	its::executor::exec(config->get_child("command.fetch"), config->get<std::string>("repository")+"/"+package+"/"+config->get<std::string>("name.file.depends"), dfile.native());
	boost::property_tree::ptree deps;
	boost::property_tree::info_parser::read_info(dfile.native(), deps);
	std::map<std::string, std::string> map;
	for (auto i = deps.begin(); i!=deps.end(); ++i)
	{
		if (map.find(i->first)!=map.end())
			throw std::runtime_error("dublicate dependencies: \""+i->first+"\"");
		map[i->first] = i->second.get_value<std::string>();
	}
	SLOG("found \""<<map.size()<<"\" dependencies");
	return map;
}

bool equal_files(const boost::filesystem::path &file1, const boost::filesystem::path &file2)
{
	// TODO unchecked
	boost::filesystem::ifstream fin1(file1, std::ios_base::in | std::ios_base::binary), fin2(file2, std::ios_base::in | std::ios_base::binary);
	char c1, c2;
	while (fin1.get(c1) && fin2.get(c2) && c1==c2);
	bool eq = c1==c2;
	eq = eq && (!fin1.get(c1)) && (!fin2.get(c2));
	SLOG("files "<<file1<<" and "<<file2<<" are "<<(eq?"equal":"not equal"));
	return eq;
}

bool its::repository::native::source_outdated(const std::string &package)
{
	its::tempfile tmp(config->get<std::string>("name.file.tmp"));
	its::executor::exec(config->get_child("command.fetch"), config->get<std::string>("repository")+"/"+package+"/"+config->get<std::string>("name.file.time"), tmp.native());
	boost::filesystem::path src = config->get<std::string>("dir.source");
	src = src/package/config->get<std::string>("name.file.time");
	return !equal_files(tmp.path(), src);
}

bool its::repository::native::package_outdated(const std::string &package)
{
	boost::filesystem::path src = config->get<std::string>("dir.source");
	src = src/package/config->get<std::string>("name.file.time");
	boost::filesystem::path pkg = config->get<std::string>("dir.package");
	pkg = pkg/package/config->get<std::string>("name.file.time");
	return !equal_files(src, pkg);
}

void its::repository::native::extract(const std::string &package, const boost::filesystem::path &destination)
{
	SLOG("starting \""<<package<<"\" "<<__func__);
	reset_dir(destination);
	//boost::filesystem::path tmp = boost::filesystem::unique_path(destination/"%%%%-%%%%-%%%%-%%%%");// TODO possible garbage
	its::tempfile tmp(destination/"%%%%-%%%%-%%%%-%%%%");
	boost::filesystem::path pkgdir = config->get<std::string>("dir.package");
	pkgdir /= package;
	reset_dir(tmp.path());
	its::executor::exec(config->get_child("command.extract"), (pkgdir/config->get<std::string>("name.file.pkg")).native(), tmp.native());
	for (auto i = boost::filesystem::directory_iterator(tmp.path()/config->get<std::string>("name.dir.pkg")); i!=boost::filesystem::directory_iterator(); ++i)
	{
		SLOG("moving "<<i->path()<<" to "<<destination/(i->path().filename()));
		boost::filesystem::rename(i->path(), destination/(i->path().filename()));
	}
	//SLOG("removing "<<tmp.path());
	//boost::filesystem::remove_all(tmp.path());
}

void its::repository::native::clean()
{
	DLOG(starting cache clean);
	reset_dir(config->get<std::string>("dir.source"));
	reset_dir(config->get<std::string>("dir.package"));
	reset_dir(config->get<std::string>("dir.tmp"));
	DLOG(cache was cleaned);
}

