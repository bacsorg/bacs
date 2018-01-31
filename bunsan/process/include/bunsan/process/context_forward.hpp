#pragma once

#include <boost/exception/to_string.hpp>

namespace bunsan {
namespace process {

class context;

}  // namespace process
}  // namespace bunsan

namespace boost {
std::string to_string(const bunsan::process::context &context);
}  // namespace boost
