#pragma once

#include <bunsan/curl/options/traits.hpp>

#include <curl/curl.h>

#include <boost/cast.hpp>

#include <chrono>

namespace bunsan {
namespace curl {
namespace options {
namespace wrapper {

template <typename Duration>
class duration;

template <typename Rep, typename Period>
class duration<std::chrono::duration<Rep, Period>> {
 public:
  using retention_policy = retention_policy::by_curl;

  using duration_type = std::chrono::duration<Rep, Period>;

  template <typename Rep_, typename Period_>
  explicit duration(const std::chrono::duration<Rep_, Period_> &duration)
      : m_data(std::chrono::duration_cast<duration_type>(duration)) {}

  explicit duration(const duration_type &duration) : m_data(duration) {}

  long data() const { return boost::numeric_cast<long>(m_data.count()); }

 private:
  duration_type m_data;
};

using seconds = duration<std::chrono::seconds>;
using milliseconds = duration<std::chrono::milliseconds>;

}  // namespace wrapper
}  // namespace options
}  // namespace curl
}  // namespace bunsan
