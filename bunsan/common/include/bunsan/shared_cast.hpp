#pragma once

#include <boost/shared_ptr.hpp>

#include <memory>

namespace bunsan {

template <typename T>
boost::shared_ptr<T> shared_cast(std::shared_ptr<T> ptr) {
  return boost::shared_ptr<T>(ptr.get(), [ptr](T *) {});
}

template <typename T>
std::shared_ptr<T> shared_cast(boost::shared_ptr<T> ptr) {
  return std::shared_ptr<T>(ptr.get(), [ptr](T *) {});
}

}  // namespace bunsan
