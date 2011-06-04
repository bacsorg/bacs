#include "repository.hpp"
#include "executor.hpp"

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/info_parser.hpp>
#include <boost/format.hpp>
#include <boost/filesystem.hpp>
#include <boost/filesystem/fstream.hpp>

#include "util.hpp"
#include "tempfile.hpp"

bunsan::pm::repository::native::native(const boost::property_tree::ptree *config_): config(config_){}

void bunsan::pm::repository::native::build(const std::string &package)
{
	bunsan::pm::repository::check_package_name(package);
	bunsan::tempfile_ptr build_dir = bunsan::tempfile::in_dir(config->get<std::string>("dir.tmp"));
	bunsan::reset_dir(build_dir->path());
	unpack(package, build_dir->path());
	configure(package, build_dir->path());
	compile(package, build_dir->path());
	pack(package, build_dir->path());
}

void bunsan::pm::repository::native::fetch(const std::string &package)
{
	bunsan::pm::repository::check_package_name(package);
	SLOG("starting \""<<package<<"\" "<<__func__);
	bunsan::executor fetcher(config->get_child("command.fetch"));
	std::string url = config->get<std::string>("repository")+"/"+package+"/";
	boost::filesystem::path output = config->get<std::string>("dir.source");
	output /= package;
	bunsan::tempfile output_tmp(output);// guard to remove output dir if something fails
	bunsan::reset_dir(output);
	fetcher(url+config->get<std::string>("name.file.src"), (output/config->get<std::string>("name.file.src")).native());
	fetcher(url+config->get<std::string>("name.file.time"), (output/config->get<std::string>("name.file.time")).native());
	fetcher(url+config->get<std::string>("name.file.depends"), (output/config->get<std::string>("name.file.depends")).native());
	output_tmp.auto_remove(false);// everythin is ok, so we don't need to remove output dir
}

void bunsan::pm::repository::native::unpack(const std::string &package, const boost::filesystem::path &build_dir)
{
	SLOG("starting \""<<package<<"\" "<<__func__);
	boost::filesystem::path build = build_dir;
	bunsan::reset_dir(build);
	bunsan::executor::exec_from(
		build,
		config->get_child("command.unpack"),
		(boost::filesystem::path(config->get<std::string>("dir.source"))/package/config->get<std::string>("name.file.src")).native(),
		build.native());
}

void bunsan::pm::repository::native::configure(const std::string &package, const boost::filesystem::path &build_dir)
{
	SLOG("starting \""<<package<<"\" "<<__func__);
	boost::filesystem::path build = build_dir;
	build = build/config->get<std::string>("name.dir.build");
	bunsan::reset_dir(build);
	bunsan::executor exec(config->get_child("command.configure"));
	exec.current_path(build).add_argument((build.parent_path()/config->get<std::string>("name.dir.src")).native());
	std::map<std::string, std::string> deps = depend_keys(package);
	for (const auto &i: deps)
	{
		bunsan::reset_dir(build/(i.second));
		extract(i.second, build/i.second);
		exec.add_argument("-DDEPEND_"+i.first+"="+(build/i.second).native());
	}
	exec();
}

void bunsan::pm::repository::native::compile(const std::string &package, const boost::filesystem::path &build_dir)
{
	SLOG("starting \""<<package<<"\" "<<__func__);
	boost::filesystem::path build = build_dir;
	build = build/config->get<std::string>("name.dir.build");
	bunsan::executor::exec_from(build, config->get_child("command.compile"));
}

void bunsan::pm::repository::native::pack(const std::string &package, const boost::filesystem::path &build_dir)
{
	SLOG("starting \""<<package<<"\" "<<__func__);
	boost::filesystem::path build = build_dir;
	build = build/config->get<std::string>("name.dir.build");
	boost::filesystem::path pkgdir = config->get<std::string>("dir.package");
	pkgdir /= package;
	bunsan::reset_dir(pkgdir);
	boost::filesystem::path srcdir = config->get<std::string>("dir.source");
	srcdir /= package;
	bunsan::executor::exec_from(build, config->get_child("command.pack"));
	boost::filesystem3::copy(build/config->get<std::string>("name.file.pkg"), pkgdir/config->get<std::string>("name.file.pkg"));
	boost::filesystem3::copy(srcdir/config->get<std::string>("name.file.time"), pkgdir/config->get<std::string>("name.file.time"));
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
	SLOG("trying to get depends for \""<<package<<"\"");
	bunsan::tempfile_ptr dfile = bunsan::tempfile::from_model(config->get<std::string>("name.file.tmp"));
	SLOG("\""<<package<<"\" depends will be downloaded to "<<dfile->path());
	bunsan::executor::exec(config->get_child("command.fetch"), config->get<std::string>("repository")+"/"+package+"/"+config->get<std::string>("name.file.depends"), dfile->native());
	boost::property_tree::ptree deps;
	boost::property_tree::info_parser::read_info(dfile->native(), deps);
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

bool equal_files(const boost::filesystem::path &file1, const boost::filesystem::path &file2)
{
	// TODO: not checke
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

bool bunsan::pm::repository::native::source_outdated(const std::string &package)
{
	bunsan::pm::repository::check_package_name(package);
	bunsan::tempfile_ptr tmp = bunsan::tempfile::from_model(config->get<std::string>("name.file.tmp"));
	bunsan::executor::exec(config->get_child("command.fetch"), config->get<std::string>("repository")+"/"+package+"/"+config->get<std::string>("name.file.time"), tmp->native());
	boost::filesystem::path src = config->get<std::string>("dir.source");
	src = src/package/config->get<std::string>("name.file.time");
	return !boost::filesystem::exists(src) || !equal_files(tmp->path(), src);
}

bool bunsan::pm::repository::native::package_outdated(const std::string &package)
{
	bunsan::pm::repository::check_package_name(package);
	boost::filesystem::path src = config->get<std::string>("dir.source");
	src = src/package/config->get<std::string>("name.file.time");
	boost::filesystem::path pkg = config->get<std::string>("dir.package");
	pkg = pkg/package/config->get<std::string>("name.file.time");
	return !boost::filesystem::exists(pkg) || !equal_files(src, pkg);
}

void bunsan::pm::repository::native::extract(const std::string &package, const boost::filesystem::path &destination)
{
	bunsan::pm::repository::check_package_name(package);
	SLOG("starting \""<<package<<"\" "<<__func__);
	bunsan::reset_dir(destination);
	bunsan::tempfile_ptr tmp = bunsan::tempfile::in_dir(destination);
	boost::filesystem::path pkgdir = config->get<std::string>("dir.package");
	pkgdir /= package;
	bunsan::reset_dir(tmp->path());
	bunsan::executor::exec(config->get_child("command.extract"), (pkgdir/config->get<std::string>("name.file.pkg")).native(), tmp->native());
	for (auto i = boost::filesystem::directory_iterator(tmp->path()/config->get<std::string>("name.dir.pkg")); i!=boost::filesystem::directory_iterator(); ++i)
	{
		SLOG("moving "<<i->path()<<" to "<<destination/(i->path().filename()));
		boost::filesystem::rename(i->path(), destination/(i->path().filename()));
	}
}

