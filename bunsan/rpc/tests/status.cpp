#define BOOST_TEST_MODULE status
#include <boost/test/unit_test.hpp>

#include <bunsan/rpc/status.hpp>

BOOST_AUTO_TEST_SUITE(status)

BOOST_AUTO_TEST_CASE(status_code) {
  BOOST_CHECK_EQUAL(to_string(grpc::OUT_OF_RANGE), "OUT_OF_RANGE");
  BOOST_CHECK_EQUAL(to_string(grpc::StatusCode(523)), "GRPC[523]");
}

BOOST_AUTO_TEST_CASE(status) {
  BOOST_CHECK_EQUAL(to_string(grpc::Status(grpc::CANCELLED, "")),
                    "CANCELLED");
  BOOST_CHECK_EQUAL(to_string(grpc::Status(grpc::CANCELLED, "message")),
                    "CANCELLED message");
}

BOOST_AUTO_TEST_SUITE_END() // status
