#pragma once

#include <boost/iterator/iterator_facade.hpp>
#include <boost/optional.hpp>
#include <boost/range/iterator_range.hpp>

#include <utility>

namespace bunsan {
namespace curl {
namespace detail {

template <typename T>
class one_step_iterator
    : public boost::iterator_facade<one_step_iterator<T>, T,
                                    boost::forward_traversal_tag> {
 public:
  /// End iterator.
  one_step_iterator() = default;
  one_step_iterator(const one_step_iterator &) = default;
  one_step_iterator(one_step_iterator &&) = default;
  one_step_iterator &operator=(const one_step_iterator &) = default;
  one_step_iterator &operator=(one_step_iterator &&) = default;

  explicit one_step_iterator(T arg) : m_value(std::move(arg)) {}

 private:
  friend class boost::iterator_core_access;

  void increment() { m_value = boost::none; }

  bool equal(const one_step_iterator &other) const {
    return m_value == other.m_value;
  }

  T &dereference() const { return *m_value; }

 private:
  boost::optional<T> m_value;
};

template <typename T>
using one_step_range = boost::iterator_range<one_step_iterator<T>>;

template <typename T>
one_step_range<T> make_one_step_range(T value) {
  return one_step_range<T>{one_step_iterator<T>(std::move(value)),
                           one_step_iterator<T>()};
}

}  // namespace detail
}  // namespace curl
}  // namespace bunsan
