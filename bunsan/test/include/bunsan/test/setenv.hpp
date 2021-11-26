#pragma once

namespace bunsan::test {

int setenv(const char *key, const char *value, bool overwrite);
int unsetenv(const char *key);

}  // namespace bunsan::test

#define BUNSAN_TEST_SETOPT(NAME, VALUE) \
  const static int NAME##_##__LINE__ =  \
      ::bunsan::test::setenv(#NAME, VALUE, true)
