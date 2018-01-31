#include <bunsan/runtime/stacktrace.hpp>

#include <bunsan/runtime/detail/format.hpp>

#include <boost/assert.hpp>
#include <boost/optional.hpp>

#include <cstdlib>

#include <windows.h>
#include <psapi.h>

namespace bunsan {
namespace runtime {

std::ostream &operator<<(std::ostream &out, const stacktrace &trace) {
  const HANDLE hProc = GetCurrentProcess();
  std::vector<HMODULE> modules(1024);
  DWORD size;
  bool has_module_list = false;
  do {
    if (has_module_list) modules.resize(modules.size() * 2);
    has_module_list = EnumProcessModules(
        hProc, modules.data(), sizeof(HMODULE) * modules.size(), &size);
    size /= sizeof(HMODULE);
  } while (has_module_list && size > modules.size());
  if (has_module_list) {
    std::vector<boost::optional<MODULEINFO>> module_info(size);
    std::vector<std::string> names(size);

    for (std::size_t i = 0; i < size; ++i) {
      MODULEINFO info;
      if (GetModuleInformation(hProc, modules[i], &info, sizeof(MODULEINFO)))
        module_info[i] = info;
    }

    for (void *const function : trace) {
      std::string fname;
      void *fbase = nullptr;
      std::ptrdiff_t module_offset = 0;
      bool found = false;
      for (std::size_t i = 0; !found && i < size; ++i) {
        if (module_info[i]) {
          char *const addr = static_cast<char *>(function);
          char *const maddr = static_cast<char *>(module_info[i]->lpBaseOfDll);
          if (maddr <= addr && addr < maddr + module_info[i]->SizeOfImage) {
            fbase = maddr;
            module_offset = addr - maddr;
            if (names[i].empty()) {
              char module_name[MAX_PATH];
              const DWORD length = GetModuleBaseNameA(
                  hProc, modules[i], module_name, sizeof(module_name));
              if (length)
                names[i].assign(module_name, length);
              else
                names[i] = "???";
            }
            fname = names[i];
            found = true;
          }
        }
      }
      if (found) {
        detail::format(out, fname.c_str(), fbase, module_offset, nullptr,
                       nullptr, 0, function);
      } else {
        detail::format_fallback(out, function);
      }
    }
  } else {
    detail::format_all_fallback(out, trace);
  }
  return out;
}

}  // namespace runtime
}  // namespace bunsan
