#define BOOST_TEST_MODULE initialization
#include <boost/test/unit_test.hpp>

#include "message_fixture.hpp"

#include <bunsan/protobuf/initialization.hpp>

BOOST_FIXTURE_TEST_SUITE(initialization, message_fixture)

BOOST_AUTO_TEST_CASE(check_initialized)
{
    BOOST_CHECK_THROW(
        bp::check_initialized(empty),
        bp::initialization_error
    );
    BOOST_CHECK_THROW(
        bp::check_initialized<bp::parse_error>(empty),
        bp::parse_error
    );
    BOOST_CHECK_THROW(
        bp::check_initialized<bp::serialize_error>(empty),
        bp::serialize_error
    );
    bp::check_initialized(multiple);
}

BOOST_AUTO_TEST_SUITE_END() // initialization
