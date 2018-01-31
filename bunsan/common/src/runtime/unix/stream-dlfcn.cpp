#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif

#include <bunsan/runtime/stacktrace.hpp>

#include <bunsan/runtime/demangle.hpp>
#include <bunsan/runtime/detail/format.hpp>

#include <dlfcn.h>

namespace bunsan {
namespace runtime {

std::ostream &operator<<(std::ostream &out, const stacktrace &trace) {
  for (void *const function : trace) {
    Dl_info info;
    if (dladdr(function, &info)) {
      detail::format(out, info.dli_fname, info.dli_fbase, 0, info.dli_sname,
                     info.dli_saddr, 0, function);
    } else {
      detail::format_fallback(out, function);
    }
  }
  return out;
}

}  // namespace runtime
}  // namespace bunsan
