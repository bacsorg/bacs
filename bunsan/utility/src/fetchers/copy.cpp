#include <bunsan/config.hpp>
#include "copy.hpp"

#include <bunsan/static_initializer.hpp>

#include <boost/filesystem/operations.hpp>

using namespace bunsan::utility;

#define BUNSAN_UTILITY_FETCHER_COPY(NAME)                       \
  BUNSAN_FACTORY_REGISTER_TOKEN(fetcher, NAME, [](resolver &) { \
    return fetcher::make_shared<fetchers::copy>();              \
  })

BUNSAN_STATIC_INITIALIZER(bunsan_utility_fetchers_copy, {
  BUNSAN_UTILITY_FETCHER_COPY(cp)
  BUNSAN_UTILITY_FETCHER_COPY(copy)
})

void fetchers::copy::fetch(const std::string &uri,
                           const boost::filesystem::path &dst) {
  try {
    boost::filesystem::copy_file(
        uri, dst, boost::filesystem::copy_option::overwrite_if_exists);
  } catch (std::exception &) {
    BOOST_THROW_EXCEPTION(fetcher_fetch_error()
                          << fetcher_fetch_error::uri(uri)
                          << fetcher_fetch_error::destination(dst)
                          << enable_nested_current());
  }
}
