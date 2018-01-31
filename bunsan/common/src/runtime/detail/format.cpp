#include <bunsan/runtime/detail/format.hpp>

#include <bunsan/runtime/demangle.hpp>

#include <iomanip>

namespace bunsan {
namespace runtime {
namespace detail {
void format_fallback(std::ostream &out, void *const addr) {
  out << std::showbase << std::hex << addr << '\n';
}

void format_all_fallback(std::ostream &out, const stacktrace &trace) {
  for (void *const function : trace) format_fallback(out, function);
}

void format(std::ostream &out, const char *const fname, const void *const fbase,
            std::ptrdiff_t module_offset, const char *const sname,
            const void *const saddr, std::ptrdiff_t offset,
            const void *const addr) {
  if (!offset && saddr)
    offset = static_cast<const char *>(addr) - static_cast<const char *>(saddr);
  if (fname || fbase) {
    if (fname)
      out << fname;
    else
      out << "module";
    if (fbase) out << " at " << std::showbase << std::hex << fbase;
    if (!module_offset && fbase)
      module_offset =
          static_cast<const char *>(addr) - static_cast<const char *>(fbase);
    if (module_offset) out << "+" << std::showbase << std::hex << module_offset;
  }
  out << '(';
  if (sname)
    out << demangle(sname);
  else
    out << "???";
  if (offset) out << "+" << std::showbase << std::hex << offset;
  out << ") [" << std::showbase << std::hex << addr << ']';
  if (saddr) out << " {saddr = " << std::showbase << std::hex << saddr << "}\n";
}

}  // namespace detail
}  // namespace runtime
}  // namespace bunsan
