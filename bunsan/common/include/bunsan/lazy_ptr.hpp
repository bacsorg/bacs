#pragma once

#include <boost/noncopyable.hpp>

#include <mutex>

namespace bunsan {

namespace detail {
template <typename T, bool Destroy>
class basic_lazy_ptr : private boost::noncopyable {
 public:
  constexpr basic_lazy_ptr() {}

  ~basic_lazy_ptr() {
    if (Destroy) delete m_ptr;
  }

  T *operator->() const {
    ensure_initialized();
    return m_ptr;
  }

  T *get() const {
    ensure_initialized();
    return m_ptr;
  }

  T &operator*() const { return *get(); }

 private:
  void initialize() const { m_ptr = new T{}; }

  void ensure_initialized() const {
    std::call_once(m_once, &basic_lazy_ptr<T, Destroy>::initialize, this);
  }

 private:
  mutable std::once_flag m_once;
  mutable T *m_ptr = nullptr;
};
}  // namespace detail

template <typename T>
using lazy_ptr = detail::basic_lazy_ptr<T, true>;

template <typename T>
using global_lazy_ptr = detail::basic_lazy_ptr<T, false>;

}  // namespace bunsan
