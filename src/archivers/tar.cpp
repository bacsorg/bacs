#include "tar.hpp"

#include <boost/filesystem/operations.hpp>

#include "bunsan/process/execute.hpp"

using namespace bunsan::utility;

bool archivers::tar::factory_reg_hook = archiver::register_new("tar",
	[](const resolver &resolver_)
	{
		archiver_ptr ptr(new tar(resolver_.find_executable("tar")));
		return ptr;
	});

archivers::tar::tar(const boost::filesystem::path &exe): m_exe(exe) {}

void archivers::tar::pack_from(
	const boost::filesystem::path &cwd,
	const boost::filesystem::path &archive,
	const boost::filesystem::path &file)
{
	bunsan::process::context ctx;
	ctx.executable(m_exe);
	ctx.argv({
			m_exe.filename().string(),
			"c"+m_format+"f",
			archive.string(),
			"-C",
			cwd.string(),
			file.string()
		});
	bunsan::process::check_sync_execute(ctx);
}

void archivers::tar::unpack(
	const boost::filesystem::path &archive,
	const boost::filesystem::path &dir)
{
	bunsan::process::context ctx;
	ctx.executable(m_exe);
	ctx.argv({
			m_exe.filename().string(),
			"x"+m_format+"f",
			boost::filesystem::absolute(archive).string(),
			"-C",
			boost::filesystem::absolute(dir).string()
		});
	bunsan::process::check_sync_execute(ctx);
}

void archivers::tar::setarg(const std::string &key, const std::string &value)
{
	if (key=="format")
		m_format = value;
	else
		BOOST_THROW_EXCEPTION(unknown_option_error(key));
}

