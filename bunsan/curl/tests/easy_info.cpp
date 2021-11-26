#define BOOST_TEST_MODULE easy_info
#include <boost/test/unit_test.hpp>

#include "easy_fixture.hpp"

#include <bunsan/curl/easy.hpp>
#include <bunsan/curl/info/network.hpp>
#include <bunsan/curl/options/http.hpp>
#include <bunsan/curl/options/network.hpp>

BOOST_FIXTURE_TEST_SUITE(easy_info, easy_fixture)

BOOST_AUTO_TEST_CASE(response_code) {
  easy.set(bunsan::curl::options::url(url_root + "/hello"));
  easy.perform();
  BOOST_CHECK_EQUAL(bunsan::curl::info::response_code(easy), 200);

  easy.set(bunsan::curl::options::url(url_root + "/response"));
  easy.set(bunsan::curl::options::httpheader(
      {"X-cURL-Response-Code: 234", "X-cURL-Response-Reason: hello world"}));
  easy.perform();
  BOOST_CHECK_EQUAL(bunsan::curl::info::response_code(easy), 234);
}

BOOST_AUTO_TEST_SUITE_END()  // easy_info
