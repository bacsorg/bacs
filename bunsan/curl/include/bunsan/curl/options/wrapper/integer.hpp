#pragma once

#include <bunsan/curl/options/traits.hpp>

#include <curl/curl.h>

#include <type_traits>

namespace bunsan {
namespace curl {
namespace options {
namespace wrapper {

template <typename T>
class integer {
 public:
  using retention_policy = retention_policy::by_curl;

  explicit integer(const T data) : m_data(data) {}

  T data() const { return m_data; }

 private:
  T m_data;
};

using long_ = integer<long>;
using curl_off_t_ = integer<::curl_off_t>;
using time_t_ = integer<time_t>;

}  // namespace wrapper
}  // namespace options
}  // namespace curl
}  // namespace bunsan
