#pragma once

#include <boost/range/iterator_range.hpp>

#include <vector>

namespace bunsan {
namespace curl {
namespace detail {

template <typename T>
using static_range =
    boost::iterator_range<typename std::vector<T>::const_iterator>;

template <typename T, T... values>
static_range<T> make_static_range() {
  static const std::vector<T> data = {values...};
  return static_range<T>{data.begin(), data.end()};
}

}  // namespace detail
}  // namespace curl
}  // namespace bunsan
