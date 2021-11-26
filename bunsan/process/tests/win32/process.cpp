#define BOOST_TEST_MODULE process
#include <boost/test/unit_test.hpp>

#include <bunsan/process/execute.hpp>

BOOST_AUTO_TEST_SUITE(process)

BOOST_AUTO_TEST_SUITE(sync_execute)

BOOST_AUTO_TEST_CASE(exit_status)
{
    BOOST_CHECK_EQUAL(
        bunsan::process::sync_execute(
            bunsan::process::context().
                executable("cmd.exe").
                arguments("cmd.exe", "/b", "10")
        ),
        10
    );
}

BOOST_AUTO_TEST_SUITE_END() // sync_execute

BOOST_AUTO_TEST_SUITE_END() // process
