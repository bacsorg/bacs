#define BOOST_TEST_MODULE lazy_ptr
#include <boost/test/unit_test.hpp>

#include <bunsan/lazy_ptr.hpp>

namespace {
struct mock {
  static int counter;
  static void reset() { counter = 0; }

  mock() { ++counter; }
  ~mock() { --counter; }

  int get() { return counter; }
};

int mock::counter = 0;

struct lazy_ptr_fixture {
  lazy_ptr_fixture() { mock::reset(); }
};
}  // namespace

BOOST_FIXTURE_TEST_SUITE(lazy_ptr, lazy_ptr_fixture)

BOOST_AUTO_TEST_CASE(lazy_ptr) {
  {
    const bunsan::lazy_ptr<mock> ptr;
    BOOST_CHECK_EQUAL(mock::counter, 0);
    BOOST_CHECK(ptr.get());
    BOOST_CHECK_EQUAL(ptr.get(), &*ptr);
    BOOST_CHECK_EQUAL(ptr->get(), 1);
    BOOST_CHECK_EQUAL(mock::counter, 1);
  }
  BOOST_CHECK_EQUAL(mock::counter, 0);
}

BOOST_AUTO_TEST_CASE(global_lazy_ptr) {
  mock *raw = nullptr;
  {
    const bunsan::global_lazy_ptr<mock> ptr;
    BOOST_CHECK_EQUAL(mock::counter, 0);
    BOOST_CHECK(ptr.get());
    BOOST_CHECK_EQUAL(ptr.get(), &*ptr);
    BOOST_CHECK_EQUAL(ptr->get(), 1);
    BOOST_CHECK_EQUAL(mock::counter, 1);
    raw = ptr.get();
  }
  BOOST_CHECK_EQUAL(mock::counter, 1);
  BOOST_CHECK_EQUAL(raw->get(), 1);
}

BOOST_AUTO_TEST_SUITE_END()  // lazy_ptr
