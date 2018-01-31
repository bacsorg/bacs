#include <bunsan/runtime/stacktrace.hpp>

#include <bunsan/runtime/demangle.hpp>
#include <bunsan/runtime/detail/format.hpp>

#include <boost/assert.hpp>
#include <boost/optional.hpp>
#include <boost/scope_exit.hpp>

#include <iomanip>

#include <cstdlib>
#include <cstring>

#include <dbghelp.h>
#include <psapi.h>
#include <windows.h>

namespace bunsan {
namespace runtime {

std::ostream &operator<<(std::ostream &out, const stacktrace &trace) {
  struct msvc_context {
    HANDLE hProcess = 0;
    bool syms_ready = false;
  };
  static const msvc_context context = []() {
    SymSetOptions(SYMOPT_DEFERRED_LOADS);
    msvc_context context = {
        .hProcess = GetCurrentProcess(),
        .syms_ready = SymInitialize(hProcess, nullptr, true)};
    return context;
  }();
  for (void *const function : trace) {
    if (context.syms_ready) {
      DWORD64 dwDisplacement = 0;
      DWORD64 dwAddress = reinterpret_cast<DWORD64>(function);
      std::vector<char> buffer(sizeof(SYMBOL_INFO) + MAX_SYM_NAME);
      PSYMBOL_INFO pSymbol = reinterpret_cast<PSYMBOL_INFO>(buffer.data());
      pSymbol->SizeOfStruct = sizeof(SYMBOL_INFO);
      pSymbol->MaxNameLen = MAX_SYM_NAME;
      if (SymFromAddr(context.hProcess, dwAddress, &dwDisplacement, pSymbol)) {
        detail::format(out, nullptr, nullptr, 0, pSymbol->Name,
                       pSymbol->Address, dwDisplacement, function);
      } else {
        detail::format_fallback(out, function);
      }
    } else {
      detail::format_fallback(out, function);
    }
  }
  return out;
}

}  // namespace runtime
}  // namespace bunsan
