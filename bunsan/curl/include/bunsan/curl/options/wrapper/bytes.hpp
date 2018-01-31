#pragma once

#include <bunsan/curl/options/traits.hpp>

#include <curl/curl.h>

#include <string>

namespace bunsan {
namespace curl {
namespace options {
namespace wrapper {

class bytes {
 public:
  using retention_policy = retention_policy::by_wrapper;

  template <typename Arg, typename... Args>
  explicit bytes(Arg &&arg, Args &&... args)
      : m_data(
            std::string(std::forward<Arg>(arg), std::forward<Args>(args)...)) {}

  const char *data() const { return m_data.data(); }

  curl_off_t size() const { return m_data.size(); }

 private:
  std::string m_data;
};

}  // namespace wrapper
}  // namespace options
}  // namespace curl
}  // namespace bunsan
