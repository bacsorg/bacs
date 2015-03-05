#include "curl.hpp"

#include <bunsan/filesystem/fstream.hpp>
#include <bunsan/process/execute.hpp>
#include <bunsan/static_initializer.hpp>

#include <boost/filesystem/operations.hpp>

using namespace bunsan::utility;

BUNSAN_STATIC_INITIALIZER(bunsan_utility_fetchers_curl,
{
    BUNSAN_FACTORY_REGISTER_TOKEN(fetcher, curl,
        [](const resolver &resolver_)
        {
            fetcher_ptr ptr(new fetchers::curl(resolver_.find_executable("curl")));
            return ptr;
        })
})

fetchers::curl::curl(const boost::filesystem::path &exe): m_exe(exe) {}

void fetchers::curl::fetch(const std::string &uri, const boost::filesystem::path &dst)
{
    try
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
        bunsan::process::check_sync_execute_with_output(ctx);
        if (!boost::filesystem::exists(dst))
        {
            bunsan::filesystem::ofstream touch(dst);
            touch.close();
        }
    }
    catch (std::exception &)
    {
        BOOST_THROW_EXCEPTION(fetcher_fetch_error() <<
                              fetcher_fetch_error::uri(uri) <<
                              fetcher_fetch_error::destination(dst) <<
                              enable_nested_current());
    }
}
