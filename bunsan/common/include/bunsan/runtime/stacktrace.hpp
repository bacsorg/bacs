#pragma once

#include <boost/stacktrace.hpp>

namespace bunsan::runtime {
using stacktrace = boost::stacktrace::stacktrace;
stacktrace get_stacktrace(const std::size_t skip = 0,
                          const std::size_t max_size = 1024);
}  // namespace bunsan::runtime
