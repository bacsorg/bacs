#include <bunsan/test/setenv.hpp>

#include <windows.h>

namespace bunsan {
namespace test {

int setenv(const char *const key, const char *const value,
           const bool overwrite) {
  int errcode = 0;
  if (!overwrite) {
    size_t envsize = 0;
    errcode = getenv_s(&envsize, NULL, 0, key);
    if (errcode || envsize) return errcode;
  }
  return _putenv_s(key, value);
}

int unsetenv(const char *const key) { return setenv(key, "", true); }

}  // namespace test
}  // namespace bunsan
