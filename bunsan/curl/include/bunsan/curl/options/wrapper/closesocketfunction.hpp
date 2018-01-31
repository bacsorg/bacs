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

struct closesocketfunction_traits {
  using wrapper_type = basic_function<closesocketfunction_traits>;

  using function_type = std::function<int(curl_socket_t item)>;
  using fail_type = std::integral_constant<int, 1>;

  static int static_call(void *clientp, curl_socket_t item) {
    const auto this_ = static_cast<const wrapper_type *>(clientp);
    return this_->call(item);
  }
};

using closesocketfunction = closesocketfunction_traits::wrapper_type;

}  // namespace wrapper
}  // namespace options
}  // namespace curl
}  // namespace bunsan
