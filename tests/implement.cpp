#define BOOST_TEST_MODULE implement
#include <boost/test/unit_test.hpp>

#include <bunsan/rpc/implement.hpp>

BOOST_AUTO_TEST_SUITE(implement)

struct service {
  grpc::Status call0(int *context, const int *request, int *response) {
    BUNSAN_RPC_IMPLEMENT_STATUS(context, request, response, {
      BOOST_CHECK_EQUAL(*context, 10);
      BOOST_CHECK_EQUAL(*request, 20);
      *response = 30;
      return grpc::Status::CANCELLED;
    })
  }
  grpc::Status call1(int *context, const int *request, int *response) {
    BUNSAN_RPC_IMPLEMENT_RESPONSE(context, request, response, {
      BOOST_CHECK_EQUAL(*context, 40);
      BOOST_CHECK_EQUAL(*request, 50);
      return 60;
    })
  }
  grpc::Status call2(int *context, const int *request, int *response) {
    BUNSAN_RPC_IMPLEMENT_STATUS(context, request, response, {
      throw std::runtime_error("hello error world");
    })
  }
};

BOOST_AUTO_TEST_CASE(calls) {
  service s;
  int c0 = 10, q0 = 20, r0, c1 = 40, q1 = 50, r1, c2 = 70, q2 = 80, r2;
  BOOST_CHECK_EQUAL(s.call0(&c0, &q0, &r0).error_code(), grpc::CANCELLED);
  BOOST_CHECK_EQUAL(r0, 30);
  BOOST_CHECK(s.call1(&c1, &q1, &r1).ok());
  BOOST_CHECK_EQUAL(r1, 60);
  const grpc::Status s2 = s.call2(&c2, &q2, &r2);
  BOOST_CHECK_EQUAL(s2.error_code(), grpc::UNKNOWN);
  BOOST_CHECK_EQUAL(s2.error_message(), "hello error world");
}

BOOST_AUTO_TEST_SUITE_END()  // implement
