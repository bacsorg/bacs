#include <bunsan/curl/detail/curl_delete.hpp>

#include <curl/curl.h>

namespace bunsan {
namespace curl {
namespace detail {

void curl_delete::operator()(void *const ptr) const { ::curl_free(ptr); }

}  // namespace detail
}  // namespace curl
}  // namespace bunsan
