#pragma once

#include <bunsan/curl/options/traits.hpp>

#include <curl/curl.h>

#include <boost/optional.hpp>

#include <string>

namespace bunsan {
namespace curl {
namespace options {
namespace wrapper {

class string {
 public:
  using retention_policy = retention_policy::by_curl;

  string(boost::none_t) : m_data(boost::none) {}

  template <typename Arg, typename... Args>
  string(Arg &&arg, Args &&... args)
      : m_data(
            std::string(std::forward<Arg>(arg), std::forward<Args>(args)...)) {}

  const char *data() const {
    if (m_data) {
      return m_data->c_str();
    } else {
      return nullptr;
    }
  }

 private:
  boost::optional<std::string> m_data;
};

}  // namespace wrapper
}  // namespace options
}  // namespace curl
}  // namespace bunsan
