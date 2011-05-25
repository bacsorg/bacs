#include "repository.hpp"
#include "executor.hpp"

#include <fstream>

#include <boost/format.hpp>
#include <boost/filesystem.hpp>

#include "util.hpp"

void flush_dir(const boost::filesystem::path &dir)
{
	if (boost::filesystem::exists(dir))
		boost::filesystem::remove_all(dir);
	boost::filesystem::create_directory(dir);
}

its::repository::native::native(const boost::property_tree::ptree *config_): config(config_){}

void its::repository::native::build(const std::string &package)
{
	unpack(package);
	configure(package);
	compile(package);
	pack(package);
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

void its::repository::native::unpack(const std::string &package)
{
	SLOG("starting \""<<package<<"\" "<<__func__);
	boost::filesystem::path build = config->get<std::string>("dir.tmp");
	build = build/package;
	flush_dir(build);
	its::executor::exec_from(
		build,
		config->get_child("command.unpack"),
		(boost::filesystem::path(config->get<std::string>("dir.source"))/package/config->get<std::string>("name.file.src")).native(),
		build.native());
}

void its::repository::native::configure(const std::string &package)
{
	SLOG("starting \""<<package<<"\" "<<__func__);
	boost::filesystem::path build = config->get<std::string>("dir.tmp");
	build = build/package/config->get<std::string>("name.dir.build");
	flush_dir(build);
	its::executor exec(config->get_child("command.configure"));
	exec.current_path(build).add_argument((build.parent_path()/config->get<std::string>("name.dir.src")).native());
	std::vector<std::string> deps = depends(package);
	for (auto i = deps.cbegin(); i!=deps.cend(); ++i)
	{
		boost::filesystem::create_directory(build/(*i));
		extract(*i, build/(*i));
		exec.add_argument("-DDEPEND_"+(*i)+"="+(build/(*i)).native());
	}
	exec();
}

void its::repository::native::compile(const std::string &package)
{
	SLOG("starting \""<<package<<"\" "<<__func__);
	boost::filesystem::path build = config->get<std::string>("dir.tmp");
	build = build/package/config->get<std::string>("name.dir.build");
	executor::exec_from(build, config->get_child("command.compile"));
}

void its::repository::native::pack(const std::string &package)
{
	SLOG("starting \""<<package<<"\" "<<__func__);
	boost::filesystem::path build = config->get<std::string>("dir.tmp");
	build = build/package/config->get<std::string>("name.dir.build");
	boost::filesystem::path pkgdir = config->get<std::string>("dir.package");
	pkgdir /= package;
	flush_dir(pkgdir);
	boost::filesystem::path srcdir = config->get<std::string>("dir.source");
	srcdir /= package;
	executor::exec_from(build, config->get_child("command.pack"));
	boost::filesystem3::copy(build/config->get<std::string>("name.file.pkg"), pkgdir/config->get<std::string>("name.file.pkg"));
	boost::filesystem3::copy(srcdir/config->get<std::string>("name.file.time"), pkgdir/config->get<std::string>("name.file.time"));
}

std::vector<std::string> its::repository::native::depends(const std::string &package)
{
	SLOG("trying to get depends for \""<<package<<"\"");
	boost::filesystem::path dfile = boost::filesystem::unique_path(config->get<std::string>("name.file.tmp"));
	SLOG("\""<<package<<"\" depends will be downloaded to "<<dfile);
	its::executor::exec(config->get_child("command.fetch"), config->get<std::string>("repository")+"/"+package+"/"+config->get<std::string>("name.file.depends"), dfile.native());
	std::vector<std::string> deps;
	std::ifstream din(dfile.native());
	std::string tstr;
	while (std::getline(din, tstr))
	{
		if (!tstr.empty())
		{
			SLOG(package<<": found dependencie=\""<<tstr<<"\"");
			deps.push_back(tstr);
		}
	}
	if (din.bad())
		throw std::runtime_error("file error [bad()] \""+dfile.native()+"\"");
	SLOG("found \""<<deps.size()<<"\" dependencies");
	return deps;
}

bool equal_files(const boost::filesystem::path &file1, const boost::filesystem::path &file2)
{
	// TODO unchecked
	std::ifstream fin1(file1.native(), std::ios_base::in | std::ios_base::binary), fin2(file2.native(), std::ios_base::in | std::ios_base::binary);
	char c1, c2;
	while (fin1.get(c1) && fin2.get(c2) && c1==c2);
	bool eq = c1==c2;
	eq = eq && (!fin1.get(c1)) && (!fin2.get(c2));
	SLOG("files "<<file1<<" and "<<file2<<" are "<<(eq?"equal":"not equal"));
	return eq;
}

bool its::repository::native::source_outdated(const std::string &package)
{
	boost::filesystem::path tmp = boost::filesystem::unique_path(config->get<std::string>("name.file.tmp"));
	its::executor::exec(config->get_child("command.fetch"), config->get<std::string>("repository")+"/"+package+"/"+config->get<std::string>("name.file.time"), tmp.native());
	boost::filesystem::path src = config->get<std::string>("dir.source");
	src = src/package/config->get<std::string>("name.file.time");
	return !equal_files(tmp, src);
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
	flush_dir(destination);
	boost::filesystem::path tmp = boost::filesystem::unique_path((destination/"%%%%_%%%%_%%%%").native());
	boost::filesystem::path pkgdir = config->get<std::string>("dir.package");
	pkgdir /= package;
	flush_dir(tmp);
	its::executor::exec(config->get_child("command.extract"), (pkgdir/config->get<std::string>("name.file.pkg")).native(), tmp.native());
	for (auto i = boost::filesystem::directory_iterator(tmp/config->get<std::string>("name.dir.pkg")); i!=boost::filesystem::directory_iterator(); ++i)
	{
		SLOG("moving "<<i->path()<<" to "<<destination/(i->path().filename()));
		boost::filesystem::rename(i->path(), destination/(i->path().filename()));
	}
	SLOG("removing "<<tmp);
	boost::filesystem::remove_all(tmp);
}

void its::repository::native::clean()
{
	DLOG(starting cache clean);
	DLOG(########);
	DLOG(# TODO #);
	DLOG(########);
}

