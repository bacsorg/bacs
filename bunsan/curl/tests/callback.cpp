#define BOOST_TEST_MODULE callback
#include <boost/test/unit_test.hpp>

#include <bunsan/curl/easy.hpp>
#include <bunsan/curl/options/callback.hpp>

#include <setjmp.h>

namespace curl = bunsan::curl;
namespace options = curl::options;
namespace wrapper = options::wrapper;

BOOST_AUTO_TEST_SUITE(callback)

struct terminate_fixture {
 public:
  [[noreturn]] static void increment_calls() {
    ++calls;
    longjmp(jmp, 1);
  }

  terminate_fixture() {
    calls = 0;
    std::set_terminate(&terminate_fixture::increment_calls);
  }

  template <typename F>
  void call_noreturn(F &&f) {
    if (!setjmp(jmp)) {
      f();
    }
  }

  ~terminate_fixture() {
    std::set_terminate(nullptr);
    BOOST_CHECK_EQUAL(calls, 1);
  }

 private:
  static thread_local std::size_t calls;
  static thread_local jmp_buf jmp;
};

thread_local std::size_t terminate_fixture::calls = 0;
thread_local jmp_buf terminate_fixture::jmp;

BOOST_AUTO_TEST_CASE(closesocketfunction) {
  const wrapper::closesocketfunction cb(
      [&](curl_socket_t) -> int { throw std::runtime_error("exception"); });
  BOOST_CHECK_EQUAL(cb.call(0), 1);
}

BOOST_FIXTURE_TEST_CASE(debugfunction, terminate_fixture) {
  const wrapper::debugfunction cb(
      [&](curl::easy &, curl::infotype, char *, std::size_t)
          -> void { throw std::runtime_error("exception"); });
  curl::easy easy;
  call_noreturn(
      [&cb, &easy]() { cb.call(easy, curl::infotype::text, nullptr, 0); });
}

BOOST_AUTO_TEST_CASE(fnmatch_function) {
  const wrapper::fnmatch_function cb(
      [&](const char *, const char *)
          -> curl::fnmatch { throw std::runtime_error("exception"); });
  BOOST_CHECK_EQUAL(cb.call("", ""), curl::fnmatch::fail);
}

BOOST_AUTO_TEST_CASE(headerfunction) {
  const wrapper::headerfunction cb(
      [&](const char *, std::size_t)
          -> std::size_t { throw std::runtime_error("exception"); });
  BOOST_CHECK_EQUAL(cb.call("", 1), 0);
}

BOOST_FIXTURE_TEST_CASE(interleavefunction, terminate_fixture) {
  const wrapper::interleavefunction cb(
      [&](const char *, std::size_t)
          -> std::size_t { throw std::runtime_error("exception"); });
  call_noreturn([&cb]() { cb.call("", 1); });
}

BOOST_AUTO_TEST_CASE(ioctlfunction) {
  const wrapper::ioctlfunction cb([&](curl::easy &, int) -> curl::ioerr {
    throw std::runtime_error("exception");
  });
  curl::easy easy;
  BOOST_CHECK_EQUAL(cb.call(easy, 0), curl::ioerr::failrestart);
}

BOOST_AUTO_TEST_CASE(opensocketfunction) {
  const wrapper::opensocketfunction cb(
      [&](curl::socktype, struct curl_sockaddr *)
          -> curl_socket_t { throw std::runtime_error("exception"); });
  BOOST_CHECK_EQUAL(cb.call(curl::socktype::ipcxn, nullptr), CURL_SOCKET_BAD);
}

BOOST_AUTO_TEST_CASE(progressfunction) {
  const wrapper::progressfunction cb(
      [&](double, double, double, double)
          -> int { throw std::runtime_error("exception"); });
  BOOST_CHECK_EQUAL(cb.call(0, 0, 0, 0), 1);
}

BOOST_AUTO_TEST_CASE(xferinfofunction) {
  const wrapper::xferinfofunction cb(
      [&](curl_off_t, curl_off_t, curl_off_t, curl_off_t)
          -> int { throw std::runtime_error("exception"); });
  BOOST_CHECK_EQUAL(cb.call(0, 0, 0, 0), 1);
}

BOOST_AUTO_TEST_CASE(readfunction) {
  const wrapper::readfunction cb([&](char *, std::size_t) -> std::size_t {
    throw std::runtime_error("exception");
  });
  BOOST_CHECK_EQUAL(cb.call(nullptr, 1), CURL_READFUNC_ABORT);
}

BOOST_AUTO_TEST_CASE(seekfunction) {
  const wrapper::seekfunction cb([&](curl_off_t, int) -> curl::seekfunc {
    throw std::runtime_error("exception");
  });
  BOOST_CHECK_EQUAL(cb.call(0, 0), curl::seekfunc::fail);
}

BOOST_AUTO_TEST_CASE(sockoptfunction) {
  const wrapper::sockoptfunction cb([&](curl_socket_t, curl::socktype) -> int {
    throw std::runtime_error("exception");
  });
  BOOST_CHECK_EQUAL(cb.call(0, curl::socktype::ipcxn), CURL_SOCKOPT_ERROR);
}

BOOST_AUTO_TEST_CASE(ssl_ctx_function) {
  const wrapper::ssl_ctx_function cb([&](curl::easy &, void *) -> CURLcode {
    throw std::runtime_error("exception");
  });
  curl::easy easy;
  BOOST_CHECK_EQUAL(cb.call(easy, nullptr), CURLE_ABORTED_BY_CALLBACK);
}

BOOST_AUTO_TEST_CASE(writefunction) {
  const wrapper::writefunction cb(
      [&](const char *, std::size_t)
          -> std::size_t { throw std::runtime_error("exception"); });
  BOOST_CHECK_EQUAL(cb.call("", 1), 0);
}

BOOST_AUTO_TEST_SUITE_END()  // callback
