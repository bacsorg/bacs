#define BOOST_TEST_MODULE memory
#include <boost/test/unit_test.hpp>

#include <bunsan/memory.hpp>

#include <boost/smart_ptr/intrusive_ref_counter.hpp>

BOOST_AUTO_TEST_SUITE(memory)

BOOST_AUTO_TEST_CASE(std_unique_ptr) {
  std::unique_ptr<int> a = bunsan::make_ptr<std::unique_ptr<int>>(1);
  BOOST_REQUIRE(a);
  BOOST_CHECK_EQUAL(*a, 1);
}

BOOST_AUTO_TEST_CASE(std_shared_ptr) {
  std::shared_ptr<int> a = bunsan::make_ptr<std::shared_ptr<int>>(2);
  BOOST_REQUIRE(a);
  BOOST_CHECK_EQUAL(*a, 2);
}

struct counted : boost::intrusive_ref_counter<counted> {
  explicit counted(const int data_) : data(data_) {}
  int data;
};

BOOST_AUTO_TEST_CASE(boost_intrusive_ptr) {
  boost::intrusive_ptr<counted> a =
      bunsan::make_ptr<boost::intrusive_ptr<counted>>(3);
  BOOST_REQUIRE(a);
  BOOST_CHECK_EQUAL(a->data, 3);
}

BOOST_AUTO_TEST_CASE(boost_shared_ptr) {
  boost::shared_ptr<int> a = bunsan::make_ptr<boost::shared_ptr<int>>(4);
  BOOST_REQUIRE(a);
  BOOST_CHECK_EQUAL(*a, 4);
}

BOOST_AUTO_TEST_SUITE_END()  // memory
