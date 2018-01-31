#pragma once

#include <bunsan/curl/detail/string_list.hpp>
#include <bunsan/curl/options/traits.hpp>

#include <curl/curl.h>

#include <boost/range/iterator_range.hpp>

#include <initializer_list>

namespace bunsan {
namespace curl {
namespace options {
namespace wrapper {

class string_list {
 public:
  using retention_policy = retention_policy::by_wrapper;

  string_list() = default;

  template <typename Range>
  explicit string_list(const Range &range)
      : m_data(boost::copy_range<curl::detail::string_list>(range)) {}

  template <typename T>
  explicit string_list(const std::initializer_list<T> &container)
      : m_data(begin(container), end(container)) {}

  const ::curl_slist *data() const { return m_data.data(); }

 private:
  curl::detail::string_list m_data;
};

}  // namespace wrapper
}  // namespace options
}  // namespace curl
}  // namespace bunsan
