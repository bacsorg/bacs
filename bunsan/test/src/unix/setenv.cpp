#include <bunsan/test/setenv.hpp>

#include <stdlib.h>

namespace bunsan {
namespace test {

int setenv(const char *const key, const char *const value,
           const bool overwrite) {
  return ::setenv(key, value, static_cast<int>(overwrite));
}

int unsetenv(const char *const key) { return ::unsetenv(key); }

}  // namespace test
}  // namespace bunsan
