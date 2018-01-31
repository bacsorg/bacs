#define BOOST_TEST_MODULE runtime
#include <boost/test/unit_test.hpp>

#include <bunsan/runtime/demangle.hpp>
#include <bunsan/runtime/stacktrace.hpp>

#include <utility>

struct X {
  struct Y {};

  template <typename... Args>
  static bunsan::runtime::stacktrace f(Args &&... args) {
    return bunsan::runtime::stacktrace::get(std::forward<Args>(args)...);
  }
};

BOOST_AUTO_TEST_SUITE(runtime)

BOOST_AUTO_TEST_CASE(demangle) {
  BOOST_CHECK_EQUAL(bunsan::runtime::type_name(X::Y()), "X::Y");
}

BOOST_AUTO_TEST_CASE(stacktrace) {
  BOOST_TEST_MESSAGE(::X::f());
  BOOST_CHECK_EQUAL(X::f(1, 1).size(), 1);
  BOOST_CHECK_EQUAL(X::f(1, 2).size(), 2);
}

BOOST_AUTO_TEST_SUITE_END()  // runtime
