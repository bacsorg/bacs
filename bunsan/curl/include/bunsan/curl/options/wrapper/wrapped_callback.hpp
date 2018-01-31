#pragma once

#include <bunsan/curl/detail/easy.hpp>
#include <bunsan/curl/detail/static_range.hpp>
#include <bunsan/curl/error.hpp>
#include <bunsan/curl/options/traits.hpp>

#include <curl/curl.h>

namespace bunsan {
namespace curl {
namespace options {
namespace wrapper {

template <CURLoption CallbackId, CURLoption DataId, typename Wrapper>
class wrapped_callback : private Wrapper {
 public:
  using retention_policy = retention_policy::by_wrapper;
  using function_type = typename Wrapper::function_type;
  using result_type = typename Wrapper::result_type;

  using Wrapper::Wrapper;

  constexpr CURLoption callback_id() const { return CallbackId; }
  constexpr CURLoption data_id() const { return DataId; }

  curl::detail::static_range<CURLoption> ids() const {
    return curl::detail::make_static_range<CURLoption, CallbackId, DataId>();
  }

  void setopt(CURL *const curl) const {
    try {
      curl::detail::easy::setopt(curl, callback_id(), Wrapper::callback());
      curl::detail::easy::setopt(curl, data_id(), Wrapper::data());
    } catch (...) {
      unsetopt(curl);
      throw;
    }
  }

  void unsetopt(CURL *const curl) const noexcept {
    ::curl_easy_setopt(curl, callback_id(), nullptr);
    ::curl_easy_setopt(curl, data_id(), nullptr);
  }
};

}  // namespace wrapper
}  // namespace options
}  // namespace curl
}  // namespace bunsan

#define BUNSAN_CURL_CALLBACK_WRAPPED(NAME, CALLBACK_ID, DATA_ID, ...) \
  using NAME = ::bunsan::curl::options::wrapper::wrapped_callback<    \
      CALLBACK_ID, DATA_ID, ::bunsan::curl::options::wrapper::__VA_ARGS__>;

#define BUNSAN_CURL_CALLBACK_WRAPPED_SIMPLE(NAME, CALLBACK_ID, DATA_ID) \
  BUNSAN_CURL_CALLBACK_WRAPPED(NAME, CALLBACK_ID, DATA_ID, NAME)
