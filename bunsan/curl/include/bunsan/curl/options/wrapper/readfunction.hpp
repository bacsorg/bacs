#pragma once

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

struct readfunction_traits {
  using wrapper_type = basic_function<readfunction_traits>;

  using function_type = std::function<std::size_t(char *ptr, std::size_t size)>;
  using fail_type = std::integral_constant<std::size_t, CURL_READFUNC_ABORT>;

  static std::size_t static_call(void *ptr, std::size_t size, std::size_t nmemb,
                                 void *userdata) {
    const auto this_ = static_cast<const wrapper_type *>(userdata);
    return this_->call(static_cast<char *>(ptr), size * nmemb);
  }
};

using readfunction = readfunction_traits::wrapper_type;

}  // namespace wrapper
}  // namespace options
}  // namespace curl
}  // namespace bunsan
