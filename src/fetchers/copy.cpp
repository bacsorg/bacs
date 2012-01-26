#include "bunsan/config.hpp"
#include "copy.hpp"

#include <boost/filesystem/operations.hpp>

#include "bunsan/util.hpp"

using namespace bunsan::utility;

#ifdef BUNSAN_UTILITY_FETCHER_COPY
#	error ASSERTION: BUNSAN_UTILITY_FETCHER_COPY is in use
#endif
#define BUNSAN_UTILITY_FETCHER_COPY(NAME) \
bool fetchers::copy::factory_reg_hook_##NAME = fetcher::register_new(#NAME, \
	[](const resolver &resolver_) \
	{ \
		fetcher_ptr ptr(new copy(resolver_)); \
		return ptr; \
	});

BUNSAN_UTILITY_FETCHER_COPY(cp)
BUNSAN_UTILITY_FETCHER_COPY(copy)

fetchers::copy::copy(const resolver &) {}

void fetchers::copy::fetch(const std::string &uri, const boost::filesystem::path &dst)
{
	boost::filesystem::copy_file(uri, dst,
		boost::filesystem::copy_option::overwrite_if_exists);
}

void fetchers::copy::setarg(const std::string &key, const std::string &value)
{
	SLOG("unknown \"copy\" setarg key: \""<<key<<"\"");
}

