#include <bunsan/curl/global.hpp>

#include <bunsan/curl/error.hpp>

namespace bunsan::curl {

global::global(const long flags) {
  const CURLcode ret = ::curl_global_init(flags);
  if (ret) BOOST_THROW_EXCEPTION(easy_error(ret));
}

global::global(const long flags, const curl_malloc_callback m,
               const curl_free_callback f, const curl_realloc_callback r,
               const curl_strdup_callback s, const curl_calloc_callback c) {
  const CURLcode ret = ::curl_global_init_mem(flags, m, f, r, s, c);
  if (ret) BOOST_THROW_EXCEPTION(easy_error(ret));
}

global::~global() { ::curl_global_cleanup(); }

const global global::static_instance;

}  // namespace bunsan::curl
