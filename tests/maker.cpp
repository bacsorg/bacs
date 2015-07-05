#define BOOST_TEST_MODULE maker
#include <boost/test/unit_test.hpp>

#include <bunsan/test/test_tools.hpp>

#include "utility_fixture.hpp"

#include <bunsan/utility/maker.hpp>

#include <boost/filesystem/operations.hpp>

BOOST_FIXTURE_TEST_SUITE(maker, utility_fixture)

BOOST_AUTO_TEST_CASE(make)
{
    MOCK_EXPECT(resolver.find_executable).once().with("make").returns("exe");
    const auto mk = bu::maker::instance("make", resolver);
    MOCK_EXPECT(executor->sync_execute).calls(
        [](const bunsan::process::context &ctx) {
            BOOST_CHECK_EQUAL(ctx.current_path(), "cwd");
            BUNSAN_IF_CHECK_EQUAL(ctx.arguments().size(), 3)
            {
                BOOST_CHECK_EQUAL(ctx.arguments()[0], "exe");
                BOOST_CHECK_EQUAL(ctx.arguments()[1], "target1");
                BOOST_CHECK_EQUAL(ctx.arguments()[2], "target2");
            }
            return 0;
        });
    mk->exec("cwd", {"target1", "target2"});
}

BOOST_AUTO_TEST_SUITE_END() // maker
