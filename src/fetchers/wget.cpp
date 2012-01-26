#include "wget.hpp"

#include <boost/filesystem/operations.hpp>

#include "bunsan/util.hpp"
#include "bunsan/process/execute.hpp"

using namespace bunsan::utility;

bool fetchers::wget::factory_reg_hook = fetcher::register_new("wget",
	[](const resolver &resolver_)
	{
		fetcher_ptr ptr(new wget(resolver_));
		return ptr;
	});

fetchers::wget::wget(const resolver &resolver_): m_resolver(resolver_) {}

void fetchers::wget::fetch(const std::string &uri, const boost::filesystem::path &dst)
{
	const boost::filesystem::path exe = m_resolver.find_executable("wget");
	bunsan::process::context ctx;
	ctx.executable(exe);
	ctx.argv({
		exe.filename().string(),
		"--output-document="+boost::filesystem::absolute(dst).string(),
		"--quiet",
		uri
		});
	bunsan::process::check_sync_execute(ctx);
}

void fetchers::wget::setarg(const std::string &key, const std::string &value)
{
	SLOG("unknown \"wget\" setarg key: \""<<key<<"\"");
}

