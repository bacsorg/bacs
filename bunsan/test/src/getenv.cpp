#include <bunsan/test/getenv.hpp>

#include <boost/test/test_tools.hpp>

#include <cstdlib>

namespace bunsan {
namespace test {

const char *getenv(const char *const key) {
  BOOST_REQUIRE(key);
  const char *value = std::getenv(key);
  BOOST_REQUIRE(value);
  return value;
}

}  // namespace test
}  // namespace bunsan
