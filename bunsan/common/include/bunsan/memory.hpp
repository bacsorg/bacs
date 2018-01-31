#pragma once

#include <boost/intrusive_ptr.hpp>
#include <boost/make_shared.hpp>
#include <boost/shared_ptr.hpp>

#include <memory>
#include <utility>

namespace bunsan {

namespace detail {
template <typename Pointer>
struct ptr_maker {};

template <typename T>
struct ptr_maker<std::unique_ptr<T>> {
  template <typename... Args>
  static std::unique_ptr<T> call(Args &&... args) {
    return std::make_unique<T>(std::forward<Args>(args)...);
  }
};

template <typename T>
struct ptr_maker<std::shared_ptr<T>> {
  template <typename... Args>
  static std::shared_ptr<T> call(Args &&... args) {
    return std::make_shared<T>(std::forward<Args>(args)...);
  }
};

template <typename T>
struct ptr_maker<boost::intrusive_ptr<T>> {
  template <typename... Args>
  static boost::intrusive_ptr<T> call(Args &&... args) {
    boost::intrusive_ptr<T> ptr(new T(std::forward<Args>(args)...));
    return ptr;
  }
};

template <typename T>
struct ptr_maker<boost::shared_ptr<T>> {
  template <typename... Args>
  static boost::shared_ptr<T> call(Args &&... args) {
    return boost::make_shared<T>(std::forward<Args>(args)...);
  }
};
}  // namespace detail

template <typename Pointer, typename... Args>
inline Pointer make_ptr(Args &&... args) {
  return detail::ptr_maker<Pointer>::call(std::forward<Args>(args)...);
}

}  // namespace bunsan
