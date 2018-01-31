#pragma once

#include <bunsan/runtime/stacktrace.hpp>

#include <ostream>

namespace bunsan {
namespace runtime {
namespace detail {

void format_fallback(std::ostream &out, void *const addr);
void format_all_fallback(std::ostream &out, const stacktrace &trace);
void format(std::ostream &out, const char *const fname, const void *const fbase,
            std::ptrdiff_t module_offset, const char *const sname,
            const void *const saddr, std::ptrdiff_t offset,
            const void *const addr);

}  // namespace detail
}  // namespace runtime
}  // namespace bunsan
