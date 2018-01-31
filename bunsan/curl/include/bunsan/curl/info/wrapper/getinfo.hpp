#pragma once

#include <bunsan/curl/easy.hpp>

#include <curl/curl.h>

namespace bunsan {
namespace curl {
namespace info {
namespace wrapper {

template <typename Wrapper>
struct info_getter {
  using result_type = typename Wrapper::result_type;

  static result_type getinfo(CURL *easy) { return Wrapper::getinfo(easy); }

  static result_type getinfo(curl::easy &easy) {
    return getinfo(easy.handle());
  }
};

}  // namespace wrapper
}  // namespace info
}  // namespace curl
}  // namespace bunsan

#define BUNSAN_CURL_GETINFO_WRAPPER(WRAPPER, INFO) \
  ::bunsan::curl::info::wrapper::WRAPPER<INFO>

#define BUNSAN_CURL_GETINFO_RESULT(WRAPPER, INFO) \
  typename BUNSAN_CURL_GETINFO_WRAPPER(WRAPPER, INFO)::result_type

#define BUNSAN_CURL_GETINFO_TEMPLATE(T, NAME, INFO, WRAPPER)        \
  inline BUNSAN_CURL_GETINFO_RESULT(WRAPPER, INFO) NAME(T easy) {   \
    return ::bunsan::curl::info::wrapper::info_getter<              \
        BUNSAN_CURL_GETINFO_WRAPPER(WRAPPER, INFO)>::getinfo(easy); \
  }

#define BUNSAN_CURL_GETINFO(NAME, INFO, WRAPPER)            \
  BUNSAN_CURL_GETINFO_TEMPLATE(CURL *, NAME, INFO, WRAPPER) \
  BUNSAN_CURL_GETINFO_TEMPLATE(::bunsan::curl::easy &, NAME, INFO, WRAPPER)
