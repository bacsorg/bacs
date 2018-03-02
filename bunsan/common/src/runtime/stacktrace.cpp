#include <bunsan/runtime/stacktrace.hpp>

namespace bunsan::runtime {
stacktrace get_stacktrace(const std::size_t skip,
                          const std::size_t max_size) {
  return stacktrace(skip + 1, max_size);
}
}  // namespace bunsan::runtime
