#define BOOST_TEST_MODULE wrapper
#include <boost/test/unit_test.hpp>

#include <bunsan/curl/options/callback.hpp>
#include <bunsan/curl/options/network.hpp>
#include <bunsan/curl/options/traits.hpp>

#include <type_traits>

namespace curl = bunsan::curl;

BOOST_AUTO_TEST_SUITE(traits)

BOOST_AUTO_TEST_CASE(retention_policy) {
  static_assert(
      std::is_same<
          curl::options::option_traits<curl::options::url>::retention_policy,
          curl::options::retention_policy::by_curl>::value,
      "url");
  static_assert(
      std::is_same<curl::options::option_traits<
                       curl::options::writefunction>::retention_policy,
                   curl::options::retention_policy::by_wrapper>::value,
      "url");
}

BOOST_AUTO_TEST_CASE(is_function) {
  static_assert(
      !curl::options::option_traits<curl::options::url>::is_function::value,
      "url");
  static_assert(curl::options::option_traits<
                    curl::options::writefunction>::is_function::value,
                "writefunction");
}

BOOST_AUTO_TEST_SUITE_END()  // traits
