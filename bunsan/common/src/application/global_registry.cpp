#include <bunsan/application/global_registry.hpp>

#include <boost/assert.hpp>

namespace bunsan {
namespace application {

global_state::global_state() {}
global_state::~global_state() {}

global_registry::global_registry() {
  // note: lock by global_registry::lock()
  for (const global_state_factory &factory : global_instance->factories)
    m_states.push_back(factory());
}

global_registry::~global_registry() {}

global_registry_ptr global_registry::lock() {
  std::unique_lock<std::mutex> lk(global_instance->lock);
  global_registry_ptr ptr = global_instance->instance.lock();
  if (!ptr) {
    ptr.reset(new global_registry);
    global_instance->instance = ptr;
  }
  BOOST_ASSERT(ptr);
  return ptr;
}

bool global_registry::register_state_factory(
    const global_state_factory &factory) {
  std::unique_lock<std::mutex> lk(global_instance->lock);
  global_instance->factories.push_back(factory);
  return true;
}

global_lazy_ptr<global_registry::global_instance_storage>
    global_registry::global_instance;

}  // namespace application
}  // namespace bunsan
