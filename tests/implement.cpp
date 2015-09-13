#define BOOST_TEST_MODULE implement
#include <boost/test/unit_test.hpp>

#include <bunsan/rpc/implement.hpp>

BOOST_AUTO_TEST_SUITE(implement)

struct service {
  grpc::Status call0(int *const context, const int *request, int *response) {
    BUNSAN_RPC_IMPLEMENT_STATUS(context, request, response, authorize, {
      BOOST_CHECK_EQUAL(*context, 10);
      BOOST_CHECK_EQUAL(*request, 20);
      *response = 30;
      return grpc::Status::CANCELLED;
    })
  }
  grpc::Status call1(int *const context, const int *request, int *response) {
    BUNSAN_RPC_IMPLEMENT_RESPONSE(context, request, response, authorize, {
      BOOST_CHECK_EQUAL(*context, 40);
      BOOST_CHECK_EQUAL(*request, 50);
      return 60;
    })
  }
  grpc::Status call2(int *context, const int *request, int *response) {
    BUNSAN_RPC_IMPLEMENT_STATUS(context, request, response, authorize, {
      throw std::runtime_error("hello error world");
    })
  }
  grpc::Status call3(int *context, const int *request, int *response) {
    BUNSAN_RPC_IMPLEMENT_STATUS(context, request, response, authorize, {
      BOOST_ERROR("Unreachable");
      return grpc::Status::OK;
    })
  }
  grpc::Status call4(int *context, const int *request, int *response) {
    BUNSAN_RPC_IMPLEMENT_RESPONSE(context, request, response, authorize, {
      BOOST_ERROR("Unreachable");
      return 0;
    })
  }
  template <typename Request, typename Response>
  grpc::Status authorize(int *const context, Request * /*request*/,
                         Response * /*response*/) {
    if (*context < 0) {
      return grpc::Status(grpc::PERMISSION_DENIED, "Permission denied");
    }
    return grpc::Status::OK;
  }
};

BOOST_AUTO_TEST_CASE(calls) {
  service srv;
  {
    int c = 10, q = 20, r;
    BOOST_CHECK_EQUAL(srv.call0(&c, &q, &r).error_code(), grpc::CANCELLED);
    BOOST_CHECK_EQUAL(r, 30);
  }
  {
    int c = 40, q = 50, r;
    BOOST_CHECK(srv.call1(&c, &q, &r).ok());
    BOOST_CHECK_EQUAL(r, 60);
  }
  {
    int c = 70, q = 80, r;
    const grpc::Status s = srv.call2(&c, &q, &r);
    BOOST_CHECK_EQUAL(s.error_code(), grpc::UNKNOWN);
    BOOST_CHECK_EQUAL(s.error_message(), "hello error world");
  }
  {
    int c = -30, q = -20, r;
    const grpc::Status s = srv.call3(&c, &q, &r);
    BOOST_CHECK_EQUAL(s.error_code(), grpc::PERMISSION_DENIED);
    BOOST_CHECK_EQUAL(s.error_message(), "Permission denied");
  }
  {
    int c = -60, q = -50, r;
    const grpc::Status s = srv.call4(&c, &q, &r);
    BOOST_CHECK_EQUAL(s.error_code(), grpc::PERMISSION_DENIED);
    BOOST_CHECK_EQUAL(s.error_message(), "Permission denied");
  }
}

BOOST_AUTO_TEST_SUITE_END()  // implement
