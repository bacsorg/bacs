#pragma once

#include <bunsan/curl/easy.hpp>
#include <bunsan/curl/options/traits.hpp>
#include <bunsan/curl/options/wrapper/basic_function.hpp>

#include <curl/curl.h>

#include <functional>
#include <type_traits>
#include <utility>

namespace bunsan {
namespace curl {
namespace options {
namespace wrapper {

struct ssl_ctx_function_traits {
  using wrapper_type = basic_function<ssl_ctx_function_traits>;

  using function_type = std::function<CURLcode(curl::easy &, void *sslctx)>;
  using fail_type = std::integral_constant<CURLcode, CURLE_ABORTED_BY_CALLBACK>;

  static CURLcode static_call(CURL *handle, void *sslctx, void *parm) {
    const auto this_ = static_cast<const wrapper_type *>(parm);
    return this_->call(curl::easy::get(handle), sslctx);
  }
};

using ssl_ctx_function = ssl_ctx_function_traits::wrapper_type;

}  // namespace wrapper
}  // namespace options
}  // namespace curl
}  // namespace bunsan
