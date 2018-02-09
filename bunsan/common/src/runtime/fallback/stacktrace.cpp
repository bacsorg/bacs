#include <bunsan/runtime/stacktrace.hpp>

#include <bunsan/runtime/demangle.hpp>
#include <bunsan/runtime/detail/format.hpp>

namespace bunsan::runtime {

std::ostream &operator<<(std::ostream &out, const stacktrace &trace) {
  detail::format_all_fallback(out, trace);
  return out;
}

}  // namespace bunsan::runtime
