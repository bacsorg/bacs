#define BOOST_TEST_MODULE revision
#include <boost/test/unit_test.hpp>

#include <bacs/archive/revision.hpp>

namespace ba = bacs::archive;

BOOST_AUTO_TEST_SUITE(revision)

BOOST_AUTO_TEST_CASE(revision) {
  ba::revision rev;
  const std::string r = rev.get();
  BOOST_CHECK(!r.empty());
  BOOST_CHECK_EQUAL(r, rev.get());
  rev.update();
  BOOST_CHECK(r != rev.get());
}

BOOST_AUTO_TEST_SUITE_END()  // revision
