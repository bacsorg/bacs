#pragma once

#include <bunsan/curl/easy.hpp>
#include <bunsan/curl/ioerr.hpp>
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

struct ioctlfunction_traits {
  using wrapper_type = basic_function<ioctlfunction_traits>;

  using function_type = std::function<curl::ioerr(curl::easy &, int cmd)>;
  using fail_type =
      std::integral_constant<curl::ioerr, curl::ioerr::failrestart>;

  static curlioerr static_call(CURL *handle, int cmd, void *clientp) {
    const auto this_ = static_cast<const wrapper_type *>(clientp);
    return static_cast<curlioerr>(this_->call(curl::easy::get(handle), cmd));
  }
};

using ioctlfunction = ioctlfunction_traits::wrapper_type;

}  // namespace wrapper
}  // namespace options
}  // namespace curl
}  // namespace bunsan
