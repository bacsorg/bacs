#pragma once

#include <bunsan/curl/error.hpp>
#include <bunsan/curl/info/wrapper/basic_wrapper.hpp>

#include <curl/curl.h>

namespace bunsan {
namespace curl {
namespace info {
namespace wrapper {

template <typename T, CURLINFO Info>
struct primitive : basic_wrapper<T, Info> {
  static T getinfo(CURL *const easy) {
    T obj;
    const CURLcode ret = ::curl_easy_getinfo(easy, Info, &obj);
    if (ret != CURLE_OK)
      BOOST_THROW_EXCEPTION(easy_error(ret, "curl_easy_getinfo"));
    return obj;
  }
};

template <CURLINFO Info>
using char_ptr = primitive<char *, Info>;

template <CURLINFO Info>
using double_ = primitive<double, Info>;

template <CURLINFO Info>
using long_ = primitive<long, Info>;

template <CURLINFO Info>
using slist_ptr = primitive<::curl_slist *, Info>;

}  // namespace wrapper
}  // namespace info
}  // namespace curl
}  // namespace bunsan
