#define BOOST_TEST_MODULE log
#include <boost/test/unit_test.hpp>

#include <bunsan/log/trivial.hpp>

#include <boost/filesystem/path.hpp>

#include <thread>

BOOST_AUTO_TEST_SUITE(log_test)

void spam_logs(const char *const msg, const int count) {
  for (int i = 0; i < count; ++i) {
    BUNSAN_LOG_INFO << msg;
  }
}

BOOST_AUTO_TEST_CASE(thread_safety) {
  constexpr int count = 100 * 1000;
  std::thread t(spam_logs, "Hello", count);
  spam_logs("World", count);
  t.join();
}

BOOST_AUTO_TEST_CASE(custom_types) {
  // must compile and not crash
  BUNSAN_LOG_TRACE << boost::filesystem::path("/some/path/trace");
  BUNSAN_LOG_DEBUG << boost::filesystem::path("/some/path/debug");
  BUNSAN_LOG_INFO << boost::filesystem::path("/some/path/info");
  BUNSAN_LOG_WARNING << boost::filesystem::path("/some/path/warning");
  BUNSAN_LOG_ERROR << boost::filesystem::path("/some/path/error");
  BUNSAN_LOG_FATAL << boost::filesystem::path("/some/path/fatal");
}

BOOST_AUTO_TEST_SUITE_END()  // log_test
