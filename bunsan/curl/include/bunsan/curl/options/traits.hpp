#pragma once

#include <type_traits>

namespace bunsan {
namespace curl {
namespace options {

namespace detail {
template <typename Option>
struct option_type
    : std::remove_cv<typename std::remove_reference<Option>::type> {};

struct is_function_checker {
  template <typename Option>
  static std::true_type check(Option *, typename Option::function_type *f);

  template <typename Option>
  static std::false_type check(Option *, ...);
};

template <typename Option>
struct is_function : decltype(detail::is_function_checker::check(
                         static_cast<Option *>(nullptr), nullptr)) {};
}  // namespace detail

namespace retention_policy {
struct by_wrapper {};
struct by_curl {};
}  // namespace retention_policy

template <typename Option>
struct option_traits {
  using option_type = typename detail::option_type<Option>::type;
  using retention_policy = typename option_type::retention_policy;
  using is_function = detail::is_function<option_type>;
};

}  // namespace options
}  // namespace curl
}  // namespace bunsan
