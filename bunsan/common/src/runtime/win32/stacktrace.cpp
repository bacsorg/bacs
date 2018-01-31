#include <bunsan/runtime/stacktrace.hpp>

#include <bunsan/runtime/demangle.hpp>
#include <bunsan/runtime/detail/format.hpp>

#include <boost/assert.hpp>
#include <boost/nowide/convert.hpp>
#include <boost/optional.hpp>
#include <boost/scope_exit.hpp>

#include <iomanip>

#include <cstdlib>
#include <cstring>

#include <windows.h>
#include <psapi.h>

#ifdef BOOST_MSVC
#include <dbghelp.h>
#endif

using boost::nowide::widen;

namespace bunsan {
namespace runtime {

stacktrace stacktrace::get(const std::size_t skip_,
                           const std::size_t max_size) {
  const std::size_t skip = skip_ + 1;  // we should skip stacktrace::get()
  stacktrace trace;
  using capture_func_type = USHORT(WINAPI *)(ULONG, ULONG, PVOID *, PULONG);
#if defined(BOOST_MSVC)
  const static capture_func_type capture_stack_trace =
      &RtlCaptureStackBackTrace;
#else
  const static capture_func_type capture_stack_trace =
      []() -> capture_func_type {
    const HMODULE h = GetModuleHandleW(widen("kernel32.dll").c_str());
    if (h)
      return (capture_func_type)GetProcAddress(h, "RtlCaptureStackBackTrace");
    return nullptr;
  }();
#endif
  constexpr std::size_t UNWIND_LIMIT = 62;  // see msdn
  if (capture_stack_trace && skip < UNWIND_LIMIT) {
    const std::size_t size = std::min(max_size, UNWIND_LIMIT - skip);
    trace.resize(size);
    const std::size_t real_size =
        capture_stack_trace(skip, size, trace.data(), 0);
    trace.resize(real_size);
  }
  return trace;
}

}  // namespace runtime
}  // namespace bunsan
