#define BOOST_TEST_MODULE comparator
#include <boost/test/unit_test.hpp>

#include "message_fixture.hpp"

#include <bunsan/protobuf/comparator.hpp>

BOOST_FIXTURE_TEST_SUITE(comparator, message_fixture)

BOOST_AUTO_TEST_SUITE(equal)

BOOST_AUTO_TEST_CASE(self) {
  BOOST_CHECK(bp::equal(empty, empty));
  BOOST_CHECK(bp::equal(only_required, only_required));
  BOOST_CHECK(bp::equal(only_optional, only_optional));
  BOOST_CHECK(bp::equal(optional, optional));
  BOOST_CHECK(bp::equal(single, single));
  BOOST_CHECK(bp::equal(multiple, multiple));
}

BOOST_AUTO_TEST_CASE(copy) {
  BOOST_CHECK(bp::equal(Message(empty), empty));
  BOOST_CHECK(bp::equal(Message(only_required), only_required));
  BOOST_CHECK(bp::equal(Message(only_optional), only_optional));
  BOOST_CHECK(bp::equal(Message(optional), optional));
  BOOST_CHECK(bp::equal(Message(single), single));
  BOOST_CHECK(bp::equal(Message(multiple), multiple));
}

BOOST_AUTO_TEST_CASE(similar) {
  MessageClone b;
  b.set_required_field(single.required_field());
  b.set_optional_field(single.optional_field());
  *b.mutable_repeated_field() = single.repeated_field();
  std::string b_data;
  BOOST_REQUIRE(b.SerializeToString(&b_data));
  BOOST_CHECK_EQUAL(b_data, single_data);
  BOOST_CHECK(!bp::equal(b, single));
}

BOOST_AUTO_TEST_CASE(different) {
  BOOST_CHECK(!bp::equal(empty, only_required));
  BOOST_CHECK(!bp::equal(empty, only_optional));
  BOOST_CHECK(!bp::equal(empty, optional));
  BOOST_CHECK(!bp::equal(empty, single));
  BOOST_CHECK(!bp::equal(empty, multiple));

  BOOST_CHECK(!bp::equal(only_required, only_optional));
  BOOST_CHECK(!bp::equal(only_required, optional));
  BOOST_CHECK(!bp::equal(only_required, single));
  BOOST_CHECK(!bp::equal(only_required, multiple));

  BOOST_CHECK(!bp::equal(only_optional, optional));
  BOOST_CHECK(!bp::equal(only_optional, single));
  BOOST_CHECK(!bp::equal(only_optional, multiple));

  BOOST_CHECK(!bp::equal(optional, single));
  BOOST_CHECK(!bp::equal(optional, multiple));

  BOOST_CHECK(!bp::equal(single, multiple));
}

BOOST_AUTO_TEST_SUITE_END()  // equal

BOOST_AUTO_TEST_SUITE_END()  // comparator
