#pragma once

#include <boost/iterator/transform_iterator.hpp>
#include <boost/range/iterator_range.hpp>

#include <initializer_list>
#include <ostream>
#include <string>
#include <vector>

namespace bunsan {
namespace curl {
namespace http {

class header {
 private:
  using values_type = std::vector<std::string>;

  class merger {
   public:
    explicit merger(const std::string *const name = nullptr) : m_name(name) {}

    merger(const merger &) = default;
    merger(merger &&) = default;
    merger &operator=(const merger &) = default;
    merger &operator=(merger &&) = default;

    std::string operator()(const std::string &value) const;

   private:
    const std::string *m_name;
  };

 public:
  using values_const_iterator = values_type::const_iterator;
  using values_const_range = boost::iterator_range<values_const_iterator>;

  using plain_const_iterator =
      boost::transform_iterator<merger, values_const_iterator>;
  using plain_const_range = boost::iterator_range<plain_const_iterator>;

 public:
  header() = default;
  header(const header &) = default;
  header(header &&) = default;
  header &operator=(const header &) = default;
  header &operator=(header &&) = default;

  template <typename Arg, typename... Args>
  header(const std::string &name, Arg &&arg, Args &&... args)
      : m_name(make_name(name)),
        m_values({std::forward<Arg>(arg), std::forward<Args>(args)...}) {}

  bool operator==(const header &h) const {
    return m_name == h.m_name && m_values == h.m_values;
  }

  const std::string &name() const { return m_name; }

  bool unique_value() const { return m_values.size() == 1; }

  /// \throws header_non_unique_value_error if !unique_value()
  const std::string &value() const;

  values_const_range values() const;

  plain_const_range plain_headers() const;

  void merge(const header &h);
  void merge(header &&h);

  template <typename Arg1, typename Arg2, typename... Args>
  void merge(Arg1 &&arg1, Arg2 &&arg2, Args &&... args) {
    merge(std::forward<Arg1>(arg1));
    merge(std::forward<Arg2>(arg2), std::forward<Args>(args)...);
  }

  static std::string make_name(const std::string &raw_name);

  /// Parse header without trailing CRLF
  static header parse(const std::string &data);

 private:
  std::string m_name;
  std::vector<std::string> m_values;
};

template <typename Arg, typename... Args>
header merge_headers(Arg &&arg, Args &&... args) {
  header h(std::forward<Arg>(arg));
  h.merge(std::forward<Args>(args)...);
  return h;
}

std::ostream &operator<<(std::ostream &out, const header &h);

}  // namespace http
}  // namespace curl
}  // namespace bunsan
