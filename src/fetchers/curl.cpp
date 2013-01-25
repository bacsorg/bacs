#include "curl.hpp"

#include "bunsan/process/execute.hpp"
#include "bunsan/enable_error_info.hpp"
#include "bunsan/filesystem/fstream.hpp"

#include <boost/filesystem/operations.hpp>

using namespace bunsan::utility;

const bool fetchers::curl::factory_reg_hook = fetcher::register_new("curl",
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
    ctx.arguments(
        m_exe.filename(),
        "--fail",
        "--output",
        boost::filesystem::absolute(dst),
        "--silent",
        uri
    );
    bunsan::process::check_sync_execute(ctx);
    if (!boost::filesystem::exists(dst))
    {
        BUNSAN_EXCEPTIONS_WRAP_BEGIN()
        {
            bunsan::filesystem::ofstream touch(dst);
            touch.close();
        }
        BUNSAN_EXCEPTIONS_WRAP_END()
    }
}
