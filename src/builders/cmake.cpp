#include "cmake.hpp"

#include "bunsan/process/execute.hpp"

using namespace bunsan::utility;

bool builders::cmake::factory_reg_hook = builder::register_new("cmake",
	[](const resolver &resolver_)
	{
		builder_ptr ptr(new cmake(resolver_));
		return ptr;
	});

builders::cmake::cmake(const resolver &resolver_):
	m_resolver(resolver_), m_cmake_exe(m_resolver.find_executable("cmake")) {}

std::vector<std::string> builders::cmake::argv_(const boost::filesystem::path &src)
{
	std::vector<std::string> argv;
	argv.push_back(m_cmake_exe.filename().string());
	argv.push_back(boost::filesystem::absolute(src).string());
#warning configuration is not supported
	//TODO config
	return argv;
}

void builders::cmake::configure_(
	const boost::filesystem::path &src,
	const boost::filesystem::path &bin)
{
	bunsan::process::context ctx;
	ctx.executable(m_cmake_exe);
	ctx.current_path(bin);
	ctx.argv(argv_(src));
	bunsan::process::check_sync_execute(ctx);
}

void builders::cmake::make_(
	const boost::filesystem::path &src,
	const boost::filesystem::path &bin)
{
	bunsan::process::context ctx;
#warning only make implementation
	if (true)
	{
		const boost::filesystem::path m_make_exe =
			m_resolver.find_executable("make");
		ctx.executable(m_make_exe);
		ctx.current_path(bin);
	}
	bunsan::process::check_sync_execute(ctx);
}

void builders::cmake::install_(
	const boost::filesystem::path &src,
	const boost::filesystem::path &bin,
	const boost::filesystem::path &root)
{
	bunsan::process::context ctx;
#warning only make implementation
	if (true)
	{
		const boost::filesystem::path m_make_exe =
			m_resolver.find_executable("make");
		ctx.executable(m_make_exe);
		ctx.current_path(bin);
		ctx.argv({
				m_make_exe.filename().string(),
				"install",
				"DESTDIR="+boost::filesystem::absolute(root).string()
			});
	}
	bunsan::process::check_sync_execute(ctx);
}

void builders::cmake::setup(const utility::config_type &config)
{
}

