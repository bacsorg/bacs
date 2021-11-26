#define BOOST_TEST_MODULE fnmatch
#include <boost/test/unit_test.hpp>

#include <bunsan/fnmatch.hpp>

BOOST_AUTO_TEST_SUITE(fnmatch)

BOOST_AUTO_TEST_CASE(translate_to_regex) {
  struct str_test_t {
    const char *input;
    const char *result;
  } test_strings[] = {
      {"[ab]cd?efg*", "[ab]cd.efg.*"},
      {"[iI][!^]abc[", "[iI][^^]abc\\["},
      {"[]abc", "\\[\\]abc"},
      {"a[]]b", "a[]]b"},
      {" [!]abc", "\\ \\[\\!\\]abc"},
      {"*g*", ".*g.*"},
      {"[ ][^abc][!abc][*.{}][\\[\\]\\]]",
       "[ ][\\^abc][^abc][*.{}][\\\\[\\\\]\\\\\\]\\]"},
      {"\\*", "\\\\.*"},
      {"???abc", "...abc"},
      {"[efghu", "\\[efghu"},
  };
  for (const auto &test : test_strings) {
    const std::string result =
        bunsan::fnmatcher::translate_to_regex(test.input);
    BOOST_CHECK_EQUAL(result, test.result);
  }
}

BOOST_AUTO_TEST_CASE(fnmatch) {
  BOOST_CHECK(bunsan::fnmatch("", ""));
  BOOST_CHECK(bunsan::fnmatch("*", ""));
  BOOST_CHECK(bunsan::fnmatch("*", "abc"));
  BOOST_CHECK(bunsan::fnmatch("abc", "abc"));
  BOOST_CHECK(!bunsan::fnmatch("", "abc"));
  BOOST_CHECK(bunsan::fnmatch("?bc", "abc"));
  BOOST_CHECK(bunsan::fnmatch("a*c", "ac"));
  BOOST_CHECK(bunsan::fnmatch("a*c", "abc"));
  BOOST_CHECK(bunsan::fnmatch("a*c", "abbc"));
  BOOST_CHECK(bunsan::fnmatch("a[bcd]e", "abe"));
  BOOST_CHECK(bunsan::fnmatch("a[bcd]e", "ace"));
  BOOST_CHECK(bunsan::fnmatch("a[bcd]e", "ade"));
  BOOST_CHECK(!bunsan::fnmatch("a[bcd]e", "afe"));
  BOOST_CHECK(bunsan::fnmatch("a[]]b", "a]b"));
}

BOOST_AUTO_TEST_CASE(border) {
  BOOST_CHECK(bunsan::fnmatch("hello", "hello"));
  BOOST_CHECK(!bunsan::fnmatch("hello", "hello world"));
  BOOST_CHECK(!bunsan::fnmatch("world", "hello world"));
}

BOOST_AUTO_TEST_CASE(fnmatch_icase) {
  BOOST_CHECK(!bunsan::fnmatch("abc", "aBc", bunsan::fnmatcher::defaults));
  BOOST_CHECK(bunsan::fnmatch("abc", "aBc", bunsan::fnmatcher::icase));
}

BOOST_AUTO_TEST_SUITE_END()  // fnmatch
