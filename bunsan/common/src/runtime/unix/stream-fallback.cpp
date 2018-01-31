#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif

#include <bunsan/runtime/stacktrace.hpp>

#include <bunsan/runtime/detail/format.hpp>

#include <boost/scope_exit.hpp>

#include <execinfo.h>

namespace bunsan {
namespace runtime {

std::ostream &operator<<(std::ostream &out, const stacktrace &trace) {
  char **strings = nullptr;
  BOOST_SCOPE_EXIT_ALL(strings) { free(strings); };
  strings = backtrace_symbols(trace.data(), trace.size());
  if (strings) {
    // TODO it is possible to parse strings[i] and get slightly better result
    for (std::size_t i = 0; i < trace.size(); ++i) out << strings[i] << '\n';
  } else {
    detail::format_all_fallback(out, trace);
  }
  return out;
}

}  // namespace runtime
}  // namespace bunsan
