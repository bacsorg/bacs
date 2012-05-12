#include "curl.hpp"

#include <boost/filesystem/fstream.hpp>
#include <boost/filesystem/operations.hpp>

#include "bunsan/process/execute.hpp"
#include "bunsan/system_error.hpp"

using namespace bunsan::utility;

bool fetchers::curl::factory_reg_hook = fetcher::register_new("curl",
    [](const resolver &resolver_)
    {
        fetcher_ptr ptr(new curl(resolver_.find_executable("curl")));
        return ptr;
    });

fetchers::curl::curl(const boost::filesystem::path &exe): m_exe(exe) {}

void fetchers::curl::fetch(const std::string &uri, const boost::filesystem::path &dst)
{
    bunsan::process::context ctx;
    ctx.executable(m_exe);
    ctx.argv({
        m_exe.filename().string(),
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

