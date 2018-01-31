#include <bunsan/process/execute.hpp>

#include <bunsan/process/error.hpp>
#include <bunsan/process/executor.hpp>

namespace bunsan {
namespace process {

int sync_execute(bunsan::process::context ctx) {
  try {
    return executor::instance()->sync_execute(std::move(ctx));
  } catch (std::exception &) {
    BOOST_THROW_EXCEPTION(sync_execute_error()
                          << sync_execute_error::context(ctx)
                          << enable_nested_current());
  }
}

}  // namespace process
}  // namespace bunsan
