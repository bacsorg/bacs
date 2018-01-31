#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif

#include <bunsan/runtime/stacktrace.hpp>

#include <boost/assert.hpp>

#include <execinfo.h>

namespace bunsan {
namespace runtime {

stacktrace stacktrace::get(const std::size_t skip_,
                           const std::size_t max_size) {
  const std::size_t skip = skip_ + 1;  // we should skip stacktrace::get()
  stacktrace trace;
  trace.resize(max_size + skip);
  const std::size_t real_size = ::backtrace(trace.data(), max_size + skip);
  BOOST_ASSERT(real_size <= max_size + skip);
  trace.resize(real_size);
  if (skip <= real_size) {
    trace.erase(trace.begin(), trace.begin() + skip);
  } else {
    trace.clear();
  }
  return trace;
}

}  // namespace runtime
}  // namespace bunsan
