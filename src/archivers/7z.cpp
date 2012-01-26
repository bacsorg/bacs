#include "7z.hpp"

#include "bunsan/util.hpp"
#include "bunsan/process/execute.hpp"

using namespace bunsan::utility;

#ifdef BUNSAN_UTILITY_ARCHIVER_7Z
#	error ASSERTION: BUNSAN_UTILITY_ARCHIVER_7Z is in use
#endif
#define BUNSAN_UTILITY_ARCHIVER_7Z(EXE) \
bool archivers::_7z::factory_reg_hook_##EXE = archiver::register_new(#EXE, \
	[](const resolver &resolver_) \
	{ \
		archiver_ptr ptr(new _7z(#EXE, resolver_)); \
		return ptr; \
	});

BUNSAN_UTILITY_ARCHIVER_7Z(7z)
BUNSAN_UTILITY_ARCHIVER_7Z(7za)
BUNSAN_UTILITY_ARCHIVER_7Z(7zr)

archivers::_7z::_7z(const char *exe, const resolver &resolver_):
	m_exe(exe), m_resolver(resolver_), m_format("7z") {}

void archivers::_7z::pack(
	const boost::filesystem::path &src,
	const boost::filesystem::path &dst)
{
	const boost::filesystem::path exe = m_resolver.find_executable(m_exe);
	bunsan::process::context ctx;
	ctx.executable(exe);
	ctx.argv({
		exe.filename().string(),
		"a",
		"-t"+m_format,
		dst.string(),
		src.string()
		});
	bunsan::process::check_sync_execute(ctx);
}

void archivers::_7z::unpack(
	const boost::filesystem::path &src,
	const boost::filesystem::path &dst)
{
	const boost::filesystem::path exe = m_resolver.find_executable(m_exe);
	bunsan::process::context ctx;
	ctx.executable(exe);
	ctx.argv({
		exe.filename().string(),
		"x",
		"-t"+m_format,
		src.string(),
		"-o"+dst.string()
		});
	bunsan::process::check_sync_execute(ctx);
}

void archivers::_7z::setarg(const std::string &key, const std::string &value)
{
	if (key=="format")
		m_format = value;
	else
		SLOG("unknown \""<<m_exe<<"\" setarg key: \""<<key<<"\"");
}

