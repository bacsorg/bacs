#include <bunsan/runtime/demangle.hpp>

#if defined(__GLIBCXX__) || defined(__GLIBCPP__)
#include <boost/scope_exit.hpp>

#include <cxxabi.h>

namespace bunsan {
namespace runtime {

std::string demangle(const char *const name) {
  char *realname = nullptr;
  BOOST_SCOPE_EXIT_ALL(realname) {
    if (realname) std::free(realname);
  };

  std::size_t len;
  int stat;

  realname = abi::__cxa_demangle(name, nullptr, &len, &stat);

  if (realname)
    return std::string(realname);
  else
    return name;
}

std::string demangle(const std::string &name) { return demangle(name.c_str()); }

}  // namespace runtime
}  // namespace bunsan

#else
namespace bunsan {
namespace runtime {
std::string demangle(const char *const name) { return name; }
std::string demangle(const std::string &name) { return name; }
}  // namespace runtime
}  // namespace bunsan
#endif
