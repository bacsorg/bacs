#pragma once

#include <bunsan/curl/error.hpp>

#include <curl/curl.h>

#include <utility>

namespace bunsan {
namespace curl {
namespace detail {
namespace easy {

template <typename T>
void setopt(CURL *const curl, const CURLoption id, T &&data) {
  const CURLcode ret = ::curl_easy_setopt(curl, id, std::forward<T>(data));
  if (ret) BOOST_THROW_EXCEPTION(easy_error(ret, "curl_easy_setopt"));
}

}  // namespace easy
}  // namespace detail
}  // namespace curl
}  // namespace bunsan
