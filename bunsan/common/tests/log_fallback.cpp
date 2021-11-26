#define BOOST_TEST_MODULE logging_fallback
#include <boost/test/unit_test.hpp>

#include <bunsan/log/fallback.hpp>

#include <boost/algorithm/string/predicate.hpp>

#include <sstream>

BOOST_AUTO_TEST_SUITE(logging_fallback)

BOOST_AUTO_TEST_CASE(stream) {
  std::ostringstream sout;
  BUNSAN_LOG_INFO_INTO(sout) << "Hello, world!";
  BOOST_CHECK(boost::algorithm::ends_with(sout.str(), "] Hello, world!\n"));
  BUNSAN_LOG_DEBUG_INTO(sout) << "Some debug info";
  BOOST_CHECK(boost::algorithm::ends_with(sout.str(), "] Some debug info\n"));
}

BOOST_AUTO_TEST_SUITE_END()  // logging_fallback
