#pragma once

#include <boost/exception/to_string.hpp>

namespace bunsan::process {
class context;
}  // namespace bunsan::process

namespace boost {
std::string to_string(const bunsan::process::context &context);
}  // namespace boost
