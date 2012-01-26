#include "curl.hpp"

#include <boost/filesystem/fstream.hpp>
#include <boost/filesystem/operations.hpp>

#include "bunsan/util.hpp"
#include "bunsan/process/execute.hpp"
#include "bunsan/system_error.hpp"

using namespace bunsan::utility;

bool fetchers::curl::factory_reg_hook = fetcher::register_new("curl",
	[](const resolver &resolver_)
	{
		fetcher_ptr ptr(new curl(resolver_));
		return ptr;
	});

fetchers::curl::curl(const resolver &resolver_): m_resolver(resolver_) {}

void fetchers::curl::fetch(const std::string &uri, const boost::filesystem::path &dst)
{
	const boost::filesystem::path exe = m_resolver.find_executable("curl");
	bunsan::process::context ctx;
	ctx.executable(exe);
	ctx.argv({
		exe.filename().string(),
		"--output",
		boost::filesystem::absolute(dst).string(),
		"--silent",
		uri
		});
	bunsan::process::check_sync_execute(ctx);
	if (!boost::filesystem::exists(dst))
	{
		boost::filesystem::ofstream touch(dst);
		if (!touch.is_open())
			bunsan::system_error(dst.string());
	}
}

void fetchers::curl::setarg(const std::string &key, const std::string &value)
{
	SLOG("unknown \"wget\" setarg key: \""<<key<<"\"");
}

