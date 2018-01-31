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

template <typename T>
struct basic_progressfunction_traits {
  using wrapper_type = basic_function<basic_progressfunction_traits<T>>;

  using function_type =
      std::function<int(T dltotal, T dlnow, T ultotal, T ulnow)>;
  using fail_type = std::integral_constant<int, 1>;

  static int static_call(void *clientp, T dltotal, T dlnow, T ultotal,
                         T ulnow) {
    const auto this_ = static_cast<const wrapper_type *>(clientp);
    return this_->call(dltotal, dlnow, ultotal, ulnow);
  }
};

template <typename T>
using basic_progressfunction =
    typename basic_progressfunction_traits<T>::wrapper_type;

using progressfunction = basic_progressfunction<double>;
using xferinfofunction = basic_progressfunction<curl_off_t>;

}  // namespace wrapper
}  // namespace options
}  // namespace curl
}  // namespace bunsan
