#include <bunsan/utility/maker.hpp>

#include <boost/io/quoted.hpp>

#include <sstream>

BUNSAN_FACTORY_DEFINE(bunsan::utility::maker)

namespace boost {
std::string to_string(const bunsan::utility::maker_error::targets &targets) {
  std::ostringstream sout;
  sout << "[" << bunsan::error::info_name(targets) << "] = {";
  for (std::size_t i = 0; i < targets.value().size(); ++i) {
    if (i) sout << ' ';
    sout << boost::io::quoted(targets.value()[i]);
  }
  sout << "}\n";
  return sout.str();
}

std::string to_string(const bunsan::utility::maker_error::flags &flags) {
  std::ostringstream sout;
  sout << "[" << bunsan::error::info_name(flags) << "] = {";
  bool first = true;
  for (const auto &i : flags.value()) {
    if (first) {
      first = false;
    } else {
      sout << ' ';
    }
    sout << boost::io::quoted(i.first) << '=' << boost::io::quoted(i.second);
  }
  sout << "}\n";
  return sout.str();
}
}  // namespace boost
