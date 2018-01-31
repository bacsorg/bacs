#pragma once

#include <bunsan/curl/fnmatch.hpp>
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

struct fnmatch_function_traits {
  using wrapper_type = basic_function<fnmatch_function_traits>;

  using function_type =
      std::function<curl::fnmatch(const char *pattern, const char *string)>;
  using fail_type = std::integral_constant<curl::fnmatch, curl::fnmatch::fail>;

  static int static_call(void *ptr, const char *pattern, const char *string) {
    const auto this_ = static_cast<const wrapper_type *>(ptr);
    return static_cast<int>(this_->call(pattern, string));
  }
};

using fnmatch_function = fnmatch_function_traits::wrapper_type;

}  // namespace wrapper
}  // namespace options
}  // namespace curl
}  // namespace bunsan
