#define BOOST_TEST_MODULE fetcher
#include <boost/test/unit_test.hpp>

#include <bunsan/testing/filesystem/read_data.hpp>
#include <bunsan/testing/filesystem/tempfiles.hpp>
#include <bunsan/testing/filesystem/write_data.hpp>
#include <bunsan/testing/test_tools.hpp>

#include "utility_fixture.hpp"

#include <bunsan/utility/fetcher.hpp>

#include <boost/filesystem/operations.hpp>

BOOST_FIXTURE_TEST_SUITE(fetcher, utility_fixture)

BOOST_AUTO_TEST_CASE(copy)
{
    const auto ft = bu::fetcher::instance("copy", resolver);
    bunsan::testing::filesystem::tempfiles tmp;
    const auto src = tmp.allocate();
    const auto dst = tmp.allocate();
    bunsan::testing::filesystem::write_data(src, "some data");
    ft->fetch(src.string(), dst);
    BOOST_CHECK_EQUAL(bunsan::testing::filesystem::read_data(dst), "some data");
}

BOOST_AUTO_TEST_CASE(curl)
{
    const auto ft = bu::fetcher::instance("curl", resolver);
    MOCK_EXPECT(executor->sync_execute).calls(
        [](const bunsan::process::context &ctx) {
            BUNSAN_IF_CHECK_EQUAL(ctx.arguments().size(), 6)
            {
                BOOST_CHECK_EQUAL(ctx.arguments()[0], "curl");
                BOOST_CHECK_EQUAL(ctx.arguments()[1], "--fail");
                BOOST_CHECK_EQUAL(ctx.arguments()[2], "--output");
                BOOST_CHECK_EQUAL(ctx.arguments()[3],
                                  boost::filesystem::current_path() / "dst");
                BOOST_CHECK_EQUAL(ctx.arguments()[4], "--silent");
                BOOST_CHECK_EQUAL(ctx.arguments()[5], "src");
            }
            return 0;
        });
    ft->fetch("src", "dst");
}

BOOST_AUTO_TEST_CASE(wget)
{
    const auto ft = bu::fetcher::instance("wget", resolver);
    MOCK_EXPECT(executor->sync_execute).calls(
        [](const bunsan::process::context &ctx) {
            BUNSAN_IF_CHECK_EQUAL(ctx.arguments().size(), 4)
            {
                BOOST_CHECK_EQUAL(ctx.arguments()[0], "wget");
                BOOST_CHECK_EQUAL(ctx.arguments()[1],
                                  "--output-document=" +
                                  (boost::filesystem::current_path() / "dst").string());
                BOOST_CHECK_EQUAL(ctx.arguments()[2], "--quiet");
                BOOST_CHECK_EQUAL(ctx.arguments()[3], "src");
            }
            return 0;
        });
    ft->fetch("src", "dst");
}

BOOST_AUTO_TEST_SUITE_END() // fetcher
