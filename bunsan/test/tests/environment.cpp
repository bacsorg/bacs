#define BOOST_TEST_MODULE environment
#include <boost/test/unit_test.hpp>

#include <bunsan/test/environment.hpp>

#include <boost/filesystem/operations.hpp>

BOOST_AUTO_TEST_SUITE(test)

BOOST_AUTO_TEST_CASE(environment) {
  BOOST_CHECK(boost::filesystem::exists(bunsan::test::dir::source()));
  BOOST_CHECK(boost::filesystem::exists(bunsan::test::dir::binary()));
  BOOST_CHECK(boost::filesystem::exists(bunsan::test::dir::tests::source()));
  BOOST_CHECK(boost::filesystem::exists(bunsan::test::dir::tests::binary()));
}

BOOST_AUTO_TEST_SUITE_END()  // test
