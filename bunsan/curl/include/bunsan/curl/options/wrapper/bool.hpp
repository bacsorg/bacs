#pragma once

#include <bunsan/curl/options/traits.hpp>

#include <curl/curl.h>

namespace bunsan {
namespace curl {
namespace options {
namespace wrapper {

template <long False, long True>
class bool_ {
 public:
  using retention_policy = retention_policy::by_curl;

 public:
  bool_() = default;

  explicit bool_(const bool data) : m_data(data) {}

  long data() const { return m_data ? True : False; }

 private:
  bool m_data = False;
};

using bool_01 = bool_<0, 1>;
using bool_02 = bool_<0, 2>;

}  // namespace wrapper
}  // namespace options
}  // namespace curl
}  // namespace bunsan
