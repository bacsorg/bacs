#pragma once

#include <bunsan/process/context.hpp>
#include <bunsan/process/error.hpp>

#include <boost/filesystem/path.hpp>
#include <boost/optional.hpp>

#include <string>
#include <utility>
#include <vector>

namespace bunsan {
namespace process {

int sync_execute(context ctx);
int sync_execute_with_output(context ctx, std::string &output);
void check_sync_execute(context ctx);

/*!
 * \note If BUNSAN_PROCESS_INHERIT environment is set
 * function behaves exactly like check_sync_execute().
 */
void check_sync_execute_with_output(context ctx);

// convenient aliases

template <typename... Args>
inline int sync_execute(Args &&... args) {
  return sync_execute(context().arguments(std::forward<Args>(args)...));
}

template <typename... Args>
inline void check_sync_execute(Args &&... args) {
  check_sync_execute(context().arguments(std::forward<Args>(args)...));
}

template <typename... Args>
inline void check_sync_execute_with_output(Args &&... args) {
  check_sync_execute_with_output(
      context().arguments(std::forward<Args>(args)...));
}

}  // namespace process
}  // namespace bunsan
