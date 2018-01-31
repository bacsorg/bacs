#include <bunsan/filesystem/error.hpp>

#include <bunsan/error.hpp>

#include <sstream>

namespace boost {

#define BUNSAN_PRINT_OPENMODE(NAME)             \
  if (openmode.value() & std::ios_base::NAME) { \
    if (first)                                  \
      first = false;                            \
    else                                        \
      sout << " | ";                            \
    sout << #NAME;                              \
  }

std::string to_string(const bunsan::filesystem::error::openmode &openmode) {
  std::ostringstream sout;
  sout << "[" << bunsan::error::info_name(openmode) << "] = ";
  bool first = true;
  BUNSAN_PRINT_OPENMODE(app)
  BUNSAN_PRINT_OPENMODE(ate)
  BUNSAN_PRINT_OPENMODE(binary)
  BUNSAN_PRINT_OPENMODE(in)
  BUNSAN_PRINT_OPENMODE(out)
  BUNSAN_PRINT_OPENMODE(trunc)
  if (first) {
    if (openmode.value())
      sout << "unknown[" << openmode.value() << ']';
    else
      sout << "none";
  }
  sout << '\n';
  return sout.str();
}

}  // namespace boost
