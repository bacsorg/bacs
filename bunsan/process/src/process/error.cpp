#include <bunsan/process/error.hpp>

#include <bunsan/process/context.hpp>

#include <boost/io/detail/quoted_manip.hpp>

#include <sstream>

namespace boost {
std::string to_string(const bunsan::process::error::context &context) {
  std::ostringstream sout;
  sout << '[' << bunsan::error::info_name(context) << "] = ";
  sout << to_string(context.value()) << '\n';
  return sout.str();
}

std::string to_string(const bunsan::process::error::arguments &arguments) {
  std::ostringstream sout;
  sout << '[' << bunsan::error::info_name(arguments) << "] = ";
  bool first = true;
  for (const std::string &argument : arguments.value()) {
    if (!first) {
      sout << ", ";
    }
    sout << argument;
    first = false;
  }
  sout << '\n';
  return sout.str();
}
}  // namespace boost
