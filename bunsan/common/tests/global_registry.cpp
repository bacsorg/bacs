#define BOOST_TEST_MODULE global_registry
#include <boost/test/unit_test.hpp>

#include <bunsan/application/global_registry.hpp>

namespace ba = bunsan::application;

namespace {
struct mock {
  static int ctor;
  static int dtor;

  mock() { ++ctor; }
  ~mock() { ++dtor; }
};

int mock::ctor = 0;
int mock::dtor = 0;
}  // namespace

BOOST_AUTO_TEST_SUITE(global_registry)

BOOST_AUTO_TEST_CASE(register_) {
  BOOST_CHECK(ba::global_registry::register_unique_state_factory<mock>());
  BOOST_CHECK_EQUAL(mock::ctor, 0);
  BOOST_CHECK_EQUAL(mock::dtor, 0);
  {
    const ba::global_registry_ptr reg = ba::global_registry::lock();
    BOOST_CHECK_EQUAL(mock::ctor, 1);
    BOOST_CHECK_EQUAL(mock::dtor, 0);
    const ba::global_registry_ptr reg2 = ba::global_registry::lock();
    BOOST_CHECK_EQUAL(mock::ctor, 1);
    BOOST_CHECK_EQUAL(mock::dtor, 0);
  }
  BOOST_CHECK_EQUAL(mock::ctor, 1);
  BOOST_CHECK_EQUAL(mock::dtor, 1);
}

BOOST_AUTO_TEST_SUITE_END()  // global_registry
