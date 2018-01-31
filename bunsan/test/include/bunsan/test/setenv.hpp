#pragma once

namespace bunsan {
namespace test {

int setenv(const char *key, const char *value, bool overwrite);
int unsetenv(const char *key);

}  // namespace test
}  // namespace bunsan

#define BUNSAN_TEST_SETOPT(NAME, VALUE) \
  const static int NAME##_##__LINE__ =  \
      ::bunsan::test::setenv(#NAME, VALUE, true)
