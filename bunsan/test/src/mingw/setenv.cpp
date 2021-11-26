#include <bunsan/test/setenv.hpp>

#include <string>

#include <cstdlib>

namespace bunsan::test {

int setenv(const char *const key, const char *const value,
           const bool overwrite) {
  if (!overwrite && std::getenv(key)) return 0;
  const std::string data = std::string(key) + "=" + std::string(value);
  return putenv(const_cast<char *>(data.c_str()));
}

int unsetenv(const char *const key) { return setenv(key, "", true); }

}  // namespace bunsan::test
