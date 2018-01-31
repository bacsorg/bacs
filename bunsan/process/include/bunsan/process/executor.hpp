#pragma once

#include <bunsan/process/context.hpp>

#include <memory>

namespace bunsan {
namespace process {

class executor : private boost::noncopyable {
 public:
  using executor_ptr = std::shared_ptr<executor>;

  virtual ~executor();

  virtual int sync_execute(context ctx) = 0;

  static void register_native();
  static void register_instance(std::shared_ptr<executor> impl);
  static executor_ptr instance();
};

using executor_ptr = executor::executor_ptr;

}  // namespace process
}  // namespace bunsan
