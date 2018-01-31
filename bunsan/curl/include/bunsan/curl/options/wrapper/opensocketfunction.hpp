#pragma once

#include <bunsan/curl/options/traits.hpp>
#include <bunsan/curl/options/wrapper/basic_function.hpp>
#include <bunsan/curl/socktype.hpp>

#include <curl/curl.h>

#include <functional>
#include <type_traits>
#include <utility>

namespace bunsan {
namespace curl {
namespace options {
namespace wrapper {

struct opensocketfunction_traits {
  using wrapper_type = basic_function<opensocketfunction_traits>;

  using function_type = std::function<curl_socket_t(
      curl::socktype purpose, struct curl_sockaddr *address)>;
  using fail_type = std::integral_constant<curl_socket_t, CURL_SOCKET_BAD>;

  static curl_socket_t static_call(void *clientp, curlsocktype purpose,
                                   struct curl_sockaddr *address) {
    const auto this_ = static_cast<const wrapper_type *>(clientp);
    return this_->call(static_cast<curl::socktype>(purpose), address);
  }
};

using opensocketfunction = opensocketfunction_traits::wrapper_type;

}  // namespace wrapper
}  // namespace options
}  // namespace curl
}  // namespace bunsan
