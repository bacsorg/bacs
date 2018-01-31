#define BOOST_TEST_MODULE base64
#include <boost/test/unit_test.hpp>

#include <bunsan/crypto/base64.hpp>

namespace base64 = bunsan::crypto::base64;

BOOST_AUTO_TEST_SUITE(base64_)

BOOST_AUTO_TEST_CASE(encode) {
  BOOST_CHECK_EQUAL(base64::encode(""), "");
  BOOST_CHECK_EQUAL(base64::encode("f"), "Zg==");
  BOOST_CHECK_EQUAL(base64::encode("fo"), "Zm8=");
  BOOST_CHECK_EQUAL(base64::encode("foo"), "Zm9v");
  BOOST_CHECK_EQUAL(base64::encode("foob"), "Zm9vYg==");
  BOOST_CHECK_EQUAL(base64::encode("fooba"), "Zm9vYmE=");
  BOOST_CHECK_EQUAL(base64::encode("foobar"), "Zm9vYmFy");
}

BOOST_AUTO_TEST_CASE(decode) {
  BOOST_CHECK_EQUAL(base64::decode(""), "");
  BOOST_CHECK_EQUAL(base64::decode("Zg=="), "f");
  BOOST_CHECK_EQUAL(base64::decode("Zm8="), "fo");
  BOOST_CHECK_EQUAL(base64::decode("Zm9v"), "foo");
  BOOST_CHECK_EQUAL(base64::decode("Zm9vYg=="), "foob");
  BOOST_CHECK_EQUAL(base64::decode("Zm9vYmE="), "fooba");
  BOOST_CHECK_EQUAL(base64::decode("Zm9vYmFy"), "foobar");
}

BOOST_AUTO_TEST_SUITE_END()  // base64_
