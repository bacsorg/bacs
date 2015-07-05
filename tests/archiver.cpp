#define BOOST_TEST_MODULE archiver
#include <boost/test/unit_test.hpp>

#include <bunsan/test/filesystem/tempfile.hpp>
#include <bunsan/test/test_tools.hpp>

#include "utility_fixture.hpp"

#include <bunsan/utility/archiver.hpp>

#include <boost/filesystem/operations.hpp>

BOOST_FIXTURE_TEST_SUITE(archiver, utility_fixture)

BOOST_AUTO_TEST_SUITE(_7z)

BOOST_AUTO_TEST_CASE(pack)
{
    bunsan::test::filesystem::tempfile tmp;
    MOCK_EXPECT(resolver.find_executable).once().with("7z").returns("exe");
    const auto ar = bu::archiver::instance("7z", resolver);
    MOCK_EXPECT(executor->sync_execute).calls(
        [&tmp](const bunsan::process::context &ctx) {
            BUNSAN_IF_CHECK_EQUAL(ctx.arguments().size(), 5)
            {
                BOOST_CHECK(!boost::filesystem::exists(tmp.path));
                BOOST_CHECK_EQUAL(ctx.arguments()[0], "exe");
                BOOST_CHECK_EQUAL(ctx.arguments()[1], "a");
                BOOST_CHECK_EQUAL(ctx.arguments()[2], "--");
                BOOST_CHECK_EQUAL(ctx.arguments()[3], tmp.path.string() + ".");
                BOOST_CHECK_EQUAL(ctx.arguments()[4], "file");
            }
            return 0;
        });
    BOOST_CHECK(boost::filesystem::exists(tmp.path));
    ar->pack(tmp.path, "file");
}

BOOST_AUTO_TEST_CASE(pack_contents)
{
    bunsan::test::filesystem::tempfile tmp;
    MOCK_EXPECT(resolver.find_executable).once().with("7z").returns("exe");
    const auto ar = bu::archiver::instance("7z", resolver);
    MOCK_EXPECT(executor->sync_execute).calls(
        [&tmp](const bunsan::process::context &ctx) {
            BUNSAN_IF_CHECK_EQUAL(ctx.arguments().size(), 5)
            {
                BOOST_CHECK(!boost::filesystem::exists(tmp.path));
                BOOST_CHECK_EQUAL(ctx.current_path(),
                                  boost::filesystem::current_path() / "dir");
                BOOST_CHECK_EQUAL(ctx.arguments()[0], "exe");
                BOOST_CHECK_EQUAL(ctx.arguments()[1], "a");
                BOOST_CHECK_EQUAL(ctx.arguments()[2], "--");
                BOOST_CHECK_EQUAL(ctx.arguments()[3], tmp.path.string() + ".");
                BOOST_CHECK_EQUAL(ctx.arguments()[4], ".");
            }
            return 0;
        });
    BOOST_CHECK(boost::filesystem::exists(tmp.path));
    ar->pack_contents(tmp.path, "dir");
}

BOOST_AUTO_TEST_CASE(unpack)
{
    MOCK_EXPECT(resolver.find_executable).once().with("7z").returns("exe");
    const auto ar = bu::archiver::instance("7z", resolver);
    MOCK_EXPECT(executor->sync_execute).calls(
        [](const bunsan::process::context &ctx) {
            BUNSAN_IF_CHECK_EQUAL(ctx.arguments().size(), 5)
            {
                BOOST_CHECK_EQUAL(ctx.arguments()[0], "exe");
                BOOST_CHECK_EQUAL(ctx.arguments()[1], "x");
                BOOST_CHECK_EQUAL(ctx.arguments()[2], "-odir");
                BOOST_CHECK_EQUAL(ctx.arguments()[3], "--");
                BOOST_CHECK_EQUAL(ctx.arguments()[4], "archive");
            }
            return 0;
        });
    ar->unpack("archive", "dir");
}

BOOST_AUTO_TEST_SUITE_END() // _7z

BOOST_AUTO_TEST_SUITE(tar)

BOOST_AUTO_TEST_CASE(pack)
{
    MOCK_EXPECT(resolver.find_executable).once().with("tar").returns("exe");
    const auto ar = bu::archiver::instance("tar", resolver);
    MOCK_EXPECT(executor->sync_execute).calls(
        [](const bunsan::process::context &ctx) {
            BUNSAN_IF_CHECK_EQUAL(ctx.arguments().size(), 8)
            {
                BOOST_CHECK_EQUAL(ctx.arguments()[0], "exe");
                BOOST_CHECK_EQUAL(ctx.arguments()[1], "--create");
                BOOST_CHECK_EQUAL(ctx.arguments()[2], "--file");
                BOOST_CHECK_EQUAL(ctx.arguments()[3],
                                  boost::filesystem::current_path() / "archive");
                BOOST_CHECK_EQUAL(ctx.arguments()[4], "--directory");
                BOOST_CHECK_EQUAL(ctx.arguments()[5], boost::filesystem::current_path());
                BOOST_CHECK_EQUAL(ctx.arguments()[6], "--");
                BOOST_CHECK_EQUAL(ctx.arguments()[7], "file");
            }
            return 0;
        });
    ar->pack("archive", "file");
}

BOOST_AUTO_TEST_CASE(pack_contents)
{
    MOCK_EXPECT(resolver.find_executable).once().with("tar").returns("exe");
    const auto ar = bu::archiver::instance("tar", resolver);
    MOCK_EXPECT(executor->sync_execute).calls(
        [](const bunsan::process::context &ctx) {
            BUNSAN_IF_CHECK_EQUAL(ctx.arguments().size(), 8)
            {
                BOOST_CHECK_EQUAL(ctx.arguments()[0], "exe");
                BOOST_CHECK_EQUAL(ctx.arguments()[1], "--create");
                BOOST_CHECK_EQUAL(ctx.arguments()[2], "--file");
                BOOST_CHECK_EQUAL(ctx.arguments()[3],
                                  boost::filesystem::current_path() / "archive");
                BOOST_CHECK_EQUAL(ctx.arguments()[4], "--directory");
                BOOST_CHECK_EQUAL(ctx.arguments()[5],
                                  boost::filesystem::current_path() / "dir");
                BOOST_CHECK_EQUAL(ctx.arguments()[6], "--");
                BOOST_CHECK_EQUAL(ctx.arguments()[7], ".");
            }
            return 0;
        });
    ar->pack_contents("archive", "dir");
}

BOOST_AUTO_TEST_CASE(unpack)
{
    MOCK_EXPECT(resolver.find_executable).once().with("tar").returns("exe");
    const auto ar = bu::archiver::instance("tar", resolver);
    MOCK_EXPECT(executor->sync_execute).calls(
        [](const bunsan::process::context &ctx) {
            BUNSAN_IF_CHECK_EQUAL(ctx.arguments().size(), 6)
            {
                BOOST_CHECK_EQUAL(ctx.arguments()[0], "exe");
                BOOST_CHECK_EQUAL(ctx.arguments()[1], "--extract");
                BOOST_CHECK_EQUAL(ctx.arguments()[2], "--file");
                BOOST_CHECK_EQUAL(ctx.arguments()[3],
                                  boost::filesystem::current_path() / "archive");
                BOOST_CHECK_EQUAL(ctx.arguments()[4], "--directory");
                BOOST_CHECK_EQUAL(ctx.arguments()[5],
                                  boost::filesystem::current_path() / "dir");
            }
            return 0;
        });
    ar->unpack("archive", "dir");
}

BOOST_AUTO_TEST_SUITE_END() // tar

BOOST_AUTO_TEST_SUITE_END() // archiver
