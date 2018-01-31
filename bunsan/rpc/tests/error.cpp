#define BOOST_TEST_MODULE error
#include <boost/test/unit_test.hpp>

#include <bunsan/rpc/error.hpp>

BOOST_AUTO_TEST_SUITE(error)

BOOST_AUTO_TEST_CASE(rpc_status) {
  const grpc::Status status(grpc::NOT_FOUND, "hello");
  BOOST_CHECK_EQUAL(to_string(bunsan::rpc::error::rpc_status(status)),
                    "[bunsan::rpc::tag_rpc_status*] = NOT_FOUND hello\n");
}

BOOST_AUTO_TEST_SUITE_END()  // error
