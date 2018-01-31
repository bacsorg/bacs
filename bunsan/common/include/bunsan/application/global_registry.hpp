#pragma once

#include <bunsan/lazy_ptr.hpp>

#include <boost/noncopyable.hpp>

#include <functional>
#include <memory>
#include <vector>

namespace bunsan {
namespace application {

class global_state : private boost::noncopyable {
 public:
  virtual ~global_state();

 protected:
  global_state();
};

template <typename T>
class global_state_any : public global_state {
 public:
  explicit global_state_any(T state) : m_state(std::move(state)) {}

  ~global_state_any() override {}

 private:
  T m_state;
};

using global_state_ptr = std::unique_ptr<global_state>;
using global_state_factory = std::function<global_state_ptr()>;

template <typename T>
global_state_ptr make_global_any_state(T state) {
  return std::make_unique<global_state_any<T>>(std::move(state));
}

template <typename T>
global_state_ptr make_global_unique_state() {
  return make_global_any_state(std::make_unique<T>());
}

class global_registry : private boost::noncopyable {
 public:
  using global_registry_ptr = std::shared_ptr<global_registry>;

 public:
  ~global_registry();

 public:
  /*!
   * Locks state, lifetime is supported by external pointers.
   * Once all pointers are destroyed registry is removed
   * and next call will create it from scratch.
   */
  static global_registry_ptr lock();

  /// \returns true
  static bool register_state_factory(const global_state_factory &factory);

  template <typename T>
  static bool register_unique_state_factory() {
    return register_state_factory(&make_global_unique_state<T>);
  }

 private:
  global_registry();

 private:
  std::vector<global_state_ptr> m_states;

 private:
  using state_factory_list = std::vector<global_state_factory>;

  struct global_instance_storage {
    std::mutex lock;
    state_factory_list factories;
    std::weak_ptr<global_registry> instance;
  };

  static global_lazy_ptr<global_instance_storage> global_instance;
};

using global_registry_ptr = global_registry::global_registry_ptr;

}  // namespace application
}  // namespace bunsan
