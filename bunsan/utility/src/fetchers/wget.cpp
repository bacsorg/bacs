#include "wget.hpp"

#include <bunsan/process/execute.hpp>
#include <bunsan/static_initializer.hpp>

#include <boost/filesystem/operations.hpp>

using namespace bunsan::utility;

BUNSAN_STATIC_INITIALIZER(bunsan_utility_fetchers_wget, {
  BUNSAN_FACTORY_REGISTER_TOKEN(
      fetcher, wget,
      [](const utility_config &, resolver &resolver_) {
        return fetcher::make_shared<fetchers::wget>(
            resolver_.find_executable("wget"));
      })
})

fetchers::wget::wget(const boost::filesystem::path &exe) : m_exe(exe) {}

void fetchers::wget::fetch(const std::string &uri,
                           const boost::filesystem::path &dst) {
  try {
    bunsan::process::context ctx;
    ctx.executable(m_exe);
    ctx.arguments(
        m_exe.filename(),
        "--output-document=" + boost::filesystem::absolute(dst).string(),
        "--quiet", uri);
    bunsan::process::check_sync_execute_with_output(ctx);
  } catch (std::exception &) {
    BOOST_THROW_EXCEPTION(fetcher_fetch_error()
                          << fetcher_fetch_error::uri(uri)
                          << fetcher_fetch_error::destination(dst)
                          << enable_nested_current());
  }
}
