#pragma once

#include <bunsan/curl/options/traits.hpp>

#include <curl/curl.h>

#include <type_traits>

namespace bunsan {
namespace curl {
namespace options {
namespace wrapper {

namespace detail {
template <typename T, T Value, typename CopyPolicy>
class literal {
 public:
  using retention_policy = CopyPolicy;

  literal() = default;

  T data() const { return Value; }
};
}  // namespace detail

template <typename T, T Value>
class enum_literal : public detail::literal<long, static_cast<long>(Value),
                                            retention_policy::by_curl> {};

template <long Value>
class long_literal
    : public detail::literal<long, Value, retention_policy::by_curl> {};

template <char... Chars>
class string_literal {
 public:
  using retention_policy = retention_policy::by_curl;

  string_literal() = default;

  const char *data() const { return m_data; }

 private:
  const char m_data[sizeof...(Chars) + 1] = {Chars..., '\0'};
};

}  // namespace wrapper
}  // namespace options
}  // namespace curl
}  // namespace bunsan
