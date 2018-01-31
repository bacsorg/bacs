#define BOOST_TEST_MODULE option_set
#include <boost/test/unit_test.hpp>

#include <bunsan/curl/detail/static_range.hpp>
#include <bunsan/curl/options/option_set.hpp>

namespace curl = bunsan::curl;
namespace options = curl::options;

static CURL *const test_curl = reinterpret_cast<CURL *>(0xdead);

struct option_set_fixture {
  CURL *const curl = test_curl;
  options::option_set set;
};

BOOST_FIXTURE_TEST_SUITE(option_set, option_set_fixture)

struct mock_option_info {
  std::size_t setopt = 0;
  std::size_t unsetopt = 0;
};

template <typename RetentionPolicy, CURLoption... Ids>
class basic_mock_option {
 public:
  using retention_policy = RetentionPolicy;

 public:
  explicit basic_mock_option(mock_option_info *const info) : m_info(info) {}

  curl::detail::static_range<CURLoption> ids() const {
    return curl::detail::make_static_range<CURLoption, Ids...>();
  }

  void setopt(CURL *const curl) const {
    BOOST_CHECK_EQUAL(curl, test_curl);
    ++m_info->setopt;
  }

  void unsetopt(CURL *const curl) const noexcept {
    BOOST_CHECK_EQUAL(curl, test_curl);
    ++m_info->unsetopt;
  }

 private:
  mock_option_info *m_info;
};

template <CURLoption... Ids>
using mock_option_by_curl =
    basic_mock_option<options::retention_policy::by_curl, Ids...>;

template <CURLoption... Ids>
using mock_option_by_wrapper =
    basic_mock_option<options::retention_policy::by_wrapper, Ids...>;

BOOST_AUTO_TEST_CASE(setopt) {
  mock_option_info info1, info2;
  set.setopt(curl, mock_option_by_curl<CURLOPT_URL>(&info1));
  set.setopt(curl, mock_option_by_wrapper<CURLOPT_POST>(&info2));
  BOOST_CHECK_EQUAL(info1.setopt, 1);
  BOOST_CHECK_EQUAL(info1.unsetopt, 0);
  BOOST_CHECK_EQUAL(info2.setopt, 1);
  BOOST_CHECK_EQUAL(info2.unsetopt, 0);
  set.setopt(curl);
  BOOST_CHECK_EQUAL(info1.setopt, 1);
  BOOST_CHECK_EQUAL(info1.unsetopt, 0);
  BOOST_CHECK_EQUAL(info2.setopt, 2);
  BOOST_CHECK_EQUAL(info2.unsetopt, 0);
}

BOOST_AUTO_TEST_CASE(clear) {
  mock_option_info info;
  set.setopt(curl, mock_option_by_curl<CURLOPT_URL>(&info));
  BOOST_CHECK_EQUAL(info.setopt, 1);
  BOOST_CHECK_EQUAL(info.unsetopt, 0);
  set.clear();
  BOOST_CHECK_EQUAL(info.setopt, 1);
  BOOST_CHECK_EQUAL(info.unsetopt, 0);
  set.setopt(curl);
  BOOST_CHECK_EQUAL(info.setopt, 1);
  BOOST_CHECK_EQUAL(info.unsetopt, 0);
}

BOOST_AUTO_TEST_SUITE(single)

BOOST_AUTO_TEST_CASE(setopt_by_curl) {
  mock_option_info info1, info2;
  set.setopt(curl, mock_option_by_curl<CURLOPT_URL>(&info1));
  set.setopt(curl, mock_option_by_curl<CURLOPT_URL>(&info2));
  BOOST_CHECK_EQUAL(info1.setopt, 1);
  BOOST_CHECK_EQUAL(info1.unsetopt, 0);
  BOOST_CHECK_EQUAL(info2.setopt, 1);
  BOOST_CHECK_EQUAL(info2.unsetopt, 0);
}

BOOST_AUTO_TEST_CASE(setopt_by_wrapper) {
  mock_option_info info1, info2;
  set.setopt(curl, mock_option_by_wrapper<CURLOPT_URL>(&info1));
  set.setopt(curl, mock_option_by_wrapper<CURLOPT_URL>(&info2));
  BOOST_CHECK_EQUAL(info1.setopt, 1);
  BOOST_CHECK_EQUAL(info1.unsetopt, 1);
  BOOST_CHECK_EQUAL(info2.setopt, 1);
  BOOST_CHECK_EQUAL(info2.unsetopt, 0);
}

BOOST_AUTO_TEST_CASE(setopt_replace_by_curl) {
  mock_option_info info1, info2;
  set.setopt(curl, mock_option_by_wrapper<CURLOPT_URL>(&info1));
  set.setopt(curl, mock_option_by_curl<CURLOPT_URL>(&info2));
  BOOST_CHECK_EQUAL(info1.setopt, 1);
  BOOST_CHECK_EQUAL(info1.unsetopt, 1);
  BOOST_CHECK_EQUAL(info2.setopt, 1);
  BOOST_CHECK_EQUAL(info2.unsetopt, 0);
}

BOOST_AUTO_TEST_CASE(setopt_replace_by_wrapper) {
  mock_option_info info1, info2;
  set.setopt(curl, mock_option_by_curl<CURLOPT_URL>(&info1));
  set.setopt(curl, mock_option_by_wrapper<CURLOPT_URL>(&info2));
  BOOST_CHECK_EQUAL(info1.setopt, 1);
  BOOST_CHECK_EQUAL(info1.unsetopt, 0);
  BOOST_CHECK_EQUAL(info2.setopt, 1);
  BOOST_CHECK_EQUAL(info2.unsetopt, 0);
}

BOOST_AUTO_TEST_SUITE_END()  // single

BOOST_AUTO_TEST_SUITE(multiple)

BOOST_AUTO_TEST_CASE(setopt_by_curl) {
  mock_option_info info1, info2;
  set.setopt(curl, mock_option_by_curl<CURLOPT_URL, CURLOPT_POST>(&info1));
  set.setopt(curl, mock_option_by_curl<CURLOPT_URL, CURLOPT_POST>(&info2));
  BOOST_CHECK_EQUAL(info1.setopt, 1);
  BOOST_CHECK_EQUAL(info1.unsetopt, 0);
  BOOST_CHECK_EQUAL(info2.setopt, 1);
  BOOST_CHECK_EQUAL(info2.unsetopt, 0);
}

BOOST_AUTO_TEST_CASE(setopt_by_wrapper) {
  mock_option_info info1, info2;
  set.setopt(curl, mock_option_by_wrapper<CURLOPT_URL, CURLOPT_POST>(&info1));
  set.setopt(curl, mock_option_by_wrapper<CURLOPT_URL, CURLOPT_POST>(&info2));
  BOOST_CHECK_EQUAL(info1.setopt, 1);
  BOOST_CHECK_EQUAL(info1.unsetopt, 1);
  BOOST_CHECK_EQUAL(info2.setopt, 1);
  BOOST_CHECK_EQUAL(info2.unsetopt, 0);
}

BOOST_AUTO_TEST_CASE(setopt_replace_by_curl) {
  mock_option_info info1, info2;
  set.setopt(curl, mock_option_by_wrapper<CURLOPT_URL, CURLOPT_POST>(&info1));
  set.setopt(curl, mock_option_by_curl<CURLOPT_URL, CURLOPT_POST>(&info2));
  BOOST_CHECK_EQUAL(info1.setopt, 1);
  BOOST_CHECK_EQUAL(info1.unsetopt, 1);
  BOOST_CHECK_EQUAL(info2.setopt, 1);
  BOOST_CHECK_EQUAL(info2.unsetopt, 0);
}

BOOST_AUTO_TEST_CASE(setopt_replace_by_wrapper) {
  mock_option_info info1, info2;
  set.setopt(curl, mock_option_by_curl<CURLOPT_URL, CURLOPT_POST>(&info1));
  set.setopt(curl, mock_option_by_wrapper<CURLOPT_URL, CURLOPT_POST>(&info2));
  BOOST_CHECK_EQUAL(info1.setopt, 1);
  BOOST_CHECK_EQUAL(info1.unsetopt, 0);
  BOOST_CHECK_EQUAL(info2.setopt, 1);
  BOOST_CHECK_EQUAL(info2.unsetopt, 0);
}

BOOST_AUTO_TEST_CASE(setopt_replace_single_by_multiple) {
  mock_option_info info1, info2, info3;
  set.setopt(curl, mock_option_by_wrapper<CURLOPT_URL, CURLOPT_POST>(&info1));
  set.setopt(curl, mock_option_by_wrapper<CURLOPT_URL>(&info2));
  set.setopt(curl, mock_option_by_wrapper<CURLOPT_POST>(&info3));
  BOOST_CHECK_EQUAL(info1.setopt, 1);
  BOOST_CHECK_EQUAL(info1.unsetopt, 1);
  BOOST_CHECK_EQUAL(info2.setopt, 1);
  BOOST_CHECK_EQUAL(info2.unsetopt, 0);
  BOOST_CHECK_EQUAL(info3.setopt, 1);
  BOOST_CHECK_EQUAL(info3.unsetopt, 0);
}

BOOST_AUTO_TEST_CASE(setopt_replace_multiple_by_single) {
  mock_option_info info1, info2, info3;
  set.setopt(curl, mock_option_by_wrapper<CURLOPT_URL>(&info1));
  set.setopt(curl, mock_option_by_wrapper<CURLOPT_POST>(&info2));
  set.setopt(curl, mock_option_by_wrapper<CURLOPT_URL, CURLOPT_POST>(&info3));
  BOOST_CHECK_EQUAL(info1.setopt, 1);
  BOOST_CHECK_EQUAL(info1.unsetopt, 1);
  BOOST_CHECK_EQUAL(info2.setopt, 1);
  BOOST_CHECK_EQUAL(info2.unsetopt, 1);
  BOOST_CHECK_EQUAL(info3.setopt, 1);
  BOOST_CHECK_EQUAL(info3.unsetopt, 0);
}

BOOST_AUTO_TEST_SUITE_END()  // multiple

BOOST_AUTO_TEST_SUITE_END()  // option_set
