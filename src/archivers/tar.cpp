#include "tar.hpp"

#include <boost/filesystem/operations.hpp>

#include "bunsan/util.hpp"
#include "bunsan/process/execute.hpp"

using namespace bunsan::utility;

bool archivers::tar::factory_reg_hook = archiver::register_new("tar",
	[](const resolver &resolver_)
	{
		archiver_ptr ptr(new tar(resolver_));
		return ptr;
	});

archivers::tar::tar(const resolver &resolver_): m_resolver(resolver_) {}

void archivers::tar::pack(
	const boost::filesystem::path &src,
	const boost::filesystem::path &dst)
{
	const boost::filesystem::path cwd = boost::filesystem::absolute(src).parent_path();
	const boost::filesystem::path src_ = src.filename();
	const boost::filesystem::path exe = m_resolver.find_executable("tar");
	bunsan::process::context ctx;
	ctx.executable(exe);
	ctx.argv({
			exe.filename().string(),
			"c"+m_format+"f",
			boost::filesystem::absolute(dst).string(),
			"-C",
			cwd.string(),
			src_.string()
		});
	bunsan::process::check_sync_execute(ctx);
}

void archivers::tar::unpack(
	const boost::filesystem::path &src,
	const boost::filesystem::path &dst)
{
	const boost::filesystem::path exe = m_resolver.find_executable("tar");
	bunsan::process::context ctx;
	ctx.executable(exe);
	ctx.argv({
			exe.filename().string(),
			"x"+m_format+"f",
			boost::filesystem::absolute(src).string(),
			"-C",
			dst.string()
		});
	bunsan::process::check_sync_execute(ctx);
}

void archivers::tar::setup(const utility::config_type &config)
{
	for (const auto &i: config)
		setarg(i.first, i.second.get_value<std::string>());
}

void archivers::tar::setarg(const std::string &key, const std::string &value)
{
	if (key=="format")
		m_format = value;
	else
		SLOG("unknown \"tar\" setarg key: \""<<key<<"\"");
}

