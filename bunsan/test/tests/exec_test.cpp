#define BOOST_TEST_MODULE ExecTest
#include <boost/test/unit_test.hpp>

#include <bunsan/test/exec_test.hpp>
#include <bunsan/test/getenv.hpp>

#include <boost/test/unit_test_monitor.hpp>

#include <cstdlib>

BOOST_AUTO_TEST_SUITE(test)

BOOST_AUTO_TEST_CASE(nothing_to_do) {
  BOOST_CHECK(std::getenv("BOOST_TEST_CATCH_SYSTEM_ERRORS"));
  BOOST_CHECK(!boost::unit_test::unit_test_monitor.p_catch_system_errors);
}

BOOST_AUTO_TEST_SUITE_END()  // test
