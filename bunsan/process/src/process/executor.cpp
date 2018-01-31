#include <bunsan/process/executor.hpp>

#include <bunsan/process/native_executor.hpp>

#include <mutex>

#include <boost/assert.hpp>

namespace bunsan {
namespace process {

static std::mutex global_lock;
static executor_ptr global_instance;

static void register_native_unlocked() {
  global_instance = std::make_shared<native_executor>();
}

executor::~executor() {}

void executor::register_native() {
  const std::lock_guard<std::mutex> lk(global_lock);
  register_native_unlocked();
  BOOST_ASSERT(global_instance);
}

void executor::register_instance(std::shared_ptr<executor> impl) {
  BOOST_ASSERT(impl);
  const std::lock_guard<std::mutex> lk(global_lock);
  global_instance = impl;
}

executor_ptr executor::instance() {
  const std::lock_guard<std::mutex> lk(global_lock);
  if (!global_instance) register_native_unlocked();
  BOOST_ASSERT(global_instance);
  return global_instance;
}

}  // namespace process
}  // namespace bunsan
