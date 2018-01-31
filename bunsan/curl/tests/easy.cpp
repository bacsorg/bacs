#define BOOST_TEST_MODULE easy
#include <boost/test/unit_test.hpp>

#include <bunsan/curl/easy.hpp>

BOOST_AUTO_TEST_SUITE(easy)

BOOST_AUTO_TEST_CASE(ctor) {
  bunsan::curl::easy e1;
  CURL *handle = e1.handle();
  BOOST_CHECK(e1);
  bunsan::curl::easy e2 = nullptr;
  BOOST_CHECK(!e2);
  bunsan::curl::easy e3 = std::move(e2);
  BOOST_CHECK(!e2);
  BOOST_CHECK(!e3);
  e3 = std::move(e1);
  BOOST_CHECK(!e1);
  BOOST_CHECK(e3);
  BOOST_CHECK_EQUAL(e3.handle(), handle);
  swap(e2, e3);
  BOOST_CHECK(e2);
  BOOST_CHECK(!e3);
  BOOST_CHECK_EQUAL(e2.handle(), handle);
  e2 = nullptr;
  BOOST_CHECK(!e2);
}

BOOST_AUTO_TEST_CASE(default_ctor) {
  bunsan::curl::easy e;
  BOOST_REQUIRE(e);
  BOOST_CHECK_EQUAL(&bunsan::curl::easy::get(e.handle()), &e);
}

BOOST_AUTO_TEST_CASE(easy_get) {
  CURL *curl;
  bunsan::curl::easy e(curl = ::curl_easy_init());
  BOOST_REQUIRE(curl);
  BOOST_REQUIRE(e);
  BOOST_CHECK_EQUAL(&bunsan::curl::easy::get(curl), &e);
  BOOST_CHECK_EQUAL(e.handle(), curl);
}

BOOST_AUTO_TEST_CASE(escaping) {
  static const std::vector<std::pair<std::string, std::string>> tests = {
      {"", ""},
      {"123", "123"},
      {"string with spaces", "string%20with%20spaces"},
      {"multiline\nstring", "multiline%0Astring"},
      {std::string("string\0with\0zeroes", 18), "string%00with%00zeroes"}};

  bunsan::curl::easy curl;

  for (const std::pair<std::string, std::string> &test : tests) {
    const std::string &unescaped = test.first;
    const std::string &escaped = test.second;
    const std::string escaped_ = curl.escape(unescaped);
    const std::string unescaped_ = curl.unescape(escaped_);
    BOOST_CHECK_EQUAL(escaped_, escaped);
    BOOST_CHECK_EQUAL(unescaped_, unescaped);
  }
}

BOOST_AUTO_TEST_SUITE_END()  // easy
