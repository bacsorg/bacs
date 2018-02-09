#include <bunsan/curl/detail/curl_delete.hpp>

#include <curl/curl.h>

namespace bunsan::curl::detail {

void curl_delete::operator()(void *const ptr) const { ::curl_free(ptr); }

}  // namespace bunsan::curl::detail
