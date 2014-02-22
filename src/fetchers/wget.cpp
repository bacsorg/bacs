#include "wget.hpp"

#include <bunsan/process/execute.hpp>

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
    try
    {
        bunsan::process::context ctx;
        ctx.executable(m_exe);
        ctx.arguments(
            m_exe.filename(),
            "--output-document=" + boost::filesystem::absolute(dst).string(),
            "--quiet",
            uri
        );
        bunsan::process::check_sync_execute_with_output(ctx);
    }
    catch (std::exception &)
    {
        BOOST_THROW_EXCEPTION(fetcher_fetch_error() <<
                              fetcher_fetch_error::uri(uri) <<
                              fetcher_fetch_error::destination(dst) <<
                              enable_nested_current());
    }
}
