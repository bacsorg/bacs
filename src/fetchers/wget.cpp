#include "wget.hpp"

#include "bunsan/process/execute.hpp"

#include <boost/filesystem/operations.hpp>

using namespace bunsan::utility;

const bool fetchers::wget::factory_reg_hook = fetcher::register_new("wget",
    [](const resolver &resolver_)
    {
        fetcher_ptr ptr(new wget(resolver_.find_executable("wget")));
        return ptr;
    });

fetchers::wget::wget(const boost::filesystem::path &exe): m_exe(exe) {}

void fetchers::wget::fetch(const std::string &uri, const boost::filesystem::path &dst)
{
    bunsan::process::context ctx;
    ctx.executable(m_exe);
    ctx.argv({
        m_exe.filename().string(),
        "--output-document="+boost::filesystem::absolute(dst).string(),
        "--quiet",
        uri
        });
    bunsan::process::check_sync_execute(ctx);
}
