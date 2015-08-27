#define BOOST_TEST_MODULE overload
#include <boost/test/unit_test.hpp>

#include <bunsan/rpc/overload.hpp>

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

BOOST_AUTO_TEST_CASE(calls) {
  service s;
  BOOST_CHECK_EQUAL(s.call0(), 10);
  BOOST_CHECK_THROW(s.call2(1, 2), error);
}

BOOST_AUTO_TEST_SUITE_END()  // overload
