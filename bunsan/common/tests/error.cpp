#define BOOST_TEST_MODULE error
#include <boost/test/unit_test.hpp>

#include <bunsan/enable_error_info.hpp>
#include <bunsan/error.hpp>
#include <bunsan/system_error.hpp>

#include <bunsan/filesystem/fstream.hpp>

#include <boost/algorithm/string/classification.hpp>
#include <boost/algorithm/string/split.hpp>

#include <vector>

#include <cerrno>

BOOST_AUTO_TEST_SUITE(error)

struct {
  template <typename Error>
  bool operator()(const Error &e) const {
    BOOST_TEST_MESSAGE(e.what());
    return true;
  }
} print;

struct {
  bool operator()(const boost::exception &e) const {
    return boost::get_error_info<bunsan::error::message>(e);
  }
} check_message;

struct {
  bool operator()(const boost::exception &e) const {
    return boost::get_error_info<bunsan::system_error::what_message>(e);
  }
} check_what_message;

struct {
  bool operator()(const boost::exception &e) const {
    return boost::get_error_info<bunsan::error::stacktrace>(e);
  }
} check_stacktrace;

BOOST_AUTO_TEST_CASE(throw_) {
  BOOST_CHECK_EXCEPTION(BOOST_THROW_EXCEPTION(bunsan::error("message")),
                        bunsan::error, check_message);
}

// for stacktrace
template <typename Error, typename... Args>
void throw_(Args &&... args) {
  throw Error(std::forward<Args>(args)...);
}

void throw0() {
  BUNSAN_EXCEPTIONS_WRAP_BEGIN() { throw_<std::runtime_error>("hello"); }
  BUNSAN_EXCEPTIONS_WRAP_END()
}

void throw1() {
  BUNSAN_EXCEPTIONS_WRAP_BEGIN() { throw_<bunsan::error>("world"); }
  BUNSAN_EXCEPTIONS_WRAP_END()
}

void throw2() {
  bunsan::filesystem::ifstream fin(
      "/definitely/not/existing/file/with/some/unpredictable/name");
}

using info = boost::error_info<struct tag_info, std::string>;
const std::string info_text = "INFO_TEXT_TO_CHECK";

bool check_info(const boost::exception &e) {
  return check_stacktrace(e) && boost::get_error_info<info>(e) &&
         *boost::get_error_info<info>(e) == info_text;
}

void throw_error_info() {
  BUNSAN_EXCEPTIONS_WRAP_BEGIN() { throw_<std::runtime_error>("some text"); }
  BUNSAN_EXCEPTIONS_WRAP_END_ERROR_INFO(info(info_text))
}

BOOST_AUTO_TEST_CASE(wrap) {
  BOOST_CHECK_EXCEPTION(throw0(), std::runtime_error, print);
  BOOST_CHECK_EXCEPTION(throw0(), boost::exception, check_stacktrace);
  BOOST_CHECK_EXCEPTION(throw1(), bunsan::error, print);
  BOOST_CHECK_EXCEPTION(throw1(), bunsan::error, check_stacktrace);
  BOOST_CHECK_EXCEPTION(throw2(), std::exception, print);
  BOOST_CHECK_EXCEPTION(throw2(), boost::exception, check_what_message);
  BOOST_CHECK_EXCEPTION(throw_error_info(), std::exception, print);
  BOOST_CHECK_EXCEPTION(throw_error_info(), boost::exception, check_info);
}

BOOST_AUTO_TEST_CASE(system_error) {
  const auto throw_ = []() {
    errno = EEXIST;
    BOOST_THROW_EXCEPTION(bunsan::system_error("action"));
  };
  const auto check = [](const bunsan::error &e) {
    BOOST_REQUIRE(e.get<bunsan::system_error::error_code>());
    BOOST_CHECK_EQUAL(e.get<bunsan::system_error::error_code>()->value(),
                      EEXIST);
    BOOST_REQUIRE(e.get<bunsan::system_error::what_message>());
    BOOST_CHECK_EQUAL(*e.get<bunsan::system_error::what_message>(), "action");
    return true;
  };
  BOOST_CHECK_EXCEPTION(throw_(), bunsan::error, check);
}

struct my_error : virtual bunsan::error {};

struct check_my_error {
  bool operator()(const std::exception &e) const {
    const std::string s = e.what();
    BOOST_TEST_MESSAGE(s);
    std::vector<std::string> msg;
    boost::algorithm::split(msg, s, boost::algorithm::is_any_of("\n"),
                            boost::algorithm::token_compress_on);
    BOOST_REQUIRE_GE(msg.size(), 2);
    BOOST_CHECK_EQUAL(msg[1],
                      "Dynamic exception type: "
                      "boost::exception_detail::clone_impl<error::my_error>");
    return true;
  }
};

BOOST_AUTO_TEST_CASE(dynamic_exception_type) {
  BOOST_CHECK_EXCEPTION(BOOST_THROW_EXCEPTION(my_error()), std::exception,
                        check_my_error());
}

BOOST_AUTO_TEST_SUITE_END()  // error
