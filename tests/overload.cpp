#define BOOST_TEST_MODULE overload
#include <boost/test/unit_test.hpp>

#include <bunsan/rpc/overload.hpp>
#include <bunsan/test/test_tools.hpp>

#include <grpc++/status.h>

BOOST_AUTO_TEST_SUITE(overload)

struct error : virtual bunsan::rpc::error {};

struct service {
  grpc::Status call0(int &x) {
    x = 10;
    return grpc::Status::OK;
  }
  int call0() { BUNSAN_RPC_OVERLOAD(call0, int, error) }
  grpc::Status call2(const int a, const int b, int &x) {
    BOOST_CHECK_EQUAL(a, 1);
    BOOST_CHECK_EQUAL(b, 2);
    x = 20;
    return grpc::Status::CANCELLED;
  }
  int call2(const int a, const int b) {
    BUNSAN_RPC_OVERLOAD(call2, int, error, a, b)
  }
};

bool check_error(const error &e) {
  const auto status = boost::get_error_info<error::rpc_status>(e);
  BUNSAN_IF_CHECK(status) {
    BOOST_CHECK_EQUAL(status->error_code(), grpc::CANCELLED);
  }
  const auto method = boost::get_error_info<error::rpc_method>(e);
  BUNSAN_IF_CHECK(method) {
    BOOST_CHECK_EQUAL(*method, "call2");
  }
  return true;
}

BOOST_AUTO_TEST_CASE(calls) {
  service s;
  BOOST_CHECK_EQUAL(s.call0(), 10);
  BOOST_CHECK_EXCEPTION(s.call2(1, 2), error, check_error);
}

BOOST_AUTO_TEST_SUITE_END()  // overload
