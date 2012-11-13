#include "bunsan/config.hpp"
#include "copy.hpp"

#include <boost/filesystem/operations.hpp>

using namespace bunsan::utility;

#ifdef BUNSAN_UTILITY_FETCHER_COPY
#   error ASSERTION: BUNSAN_UTILITY_FETCHER_COPY is in use
#endif
#define BUNSAN_UTILITY_FETCHER_COPY(NAME) \
const bool fetchers::copy::factory_reg_hook_##NAME = fetcher::register_new(#NAME, \
    [](const resolver &) \
    { \
        fetcher_ptr ptr(new copy); \
        return ptr; \
    });

BUNSAN_UTILITY_FETCHER_COPY(cp)
BUNSAN_UTILITY_FETCHER_COPY(copy)

void fetchers::copy::fetch(const std::string &uri, const boost::filesystem::path &dst)
{
    boost::filesystem::copy_file(uri, dst,
        boost::filesystem::copy_option::overwrite_if_exists);
}
