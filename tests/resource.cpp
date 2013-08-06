#define BOOST_TEST_MODULE resource
#include <boost/test/unit_test.hpp>

#include <bacs/problem/resource/parse.hpp>

BOOST_AUTO_TEST_SUITE(resource)

BOOST_AUTO_TEST_SUITE(parse_)

namespace parse = bacs::problem::resource::parse;

BOOST_AUTO_TEST_CASE(time_millis)
{
    BOOST_CHECK_EQUAL(parse::time_millis("1"), 1000);
    BOOST_CHECK_EQUAL(parse::time_millis("1s"), 1000);
    BOOST_CHECK_EQUAL(parse::time_millis("10s"), 10000);
    BOOST_CHECK_EQUAL(parse::time_millis("10ms"), 10);
    BOOST_CHECK_EQUAL(parse::time_millis("10hs"), 10 * 100 * 1000);
    BOOST_CHECK_THROW(parse::time_millis("10 hs"), parse::time_error);
}

BOOST_AUTO_TEST_CASE(memory_bytes)
{
    BOOST_CHECK_EQUAL(parse::memory_bytes("1"), 1);
    BOOST_CHECK_EQUAL(parse::memory_bytes("1024"), 1024);
    BOOST_CHECK_EQUAL(parse::memory_bytes("15B"), 15);
    BOOST_CHECK_EQUAL(parse::memory_bytes("1KiB"), 1024);
    BOOST_CHECK_EQUAL(parse::memory_bytes("1MiB"), 1024 * 1024);
    BOOST_CHECK_THROW(parse::memory_bytes("10 B"), parse::memory_error);
}

BOOST_AUTO_TEST_SUITE_END() // parse

BOOST_AUTO_TEST_SUITE_END() // resource
