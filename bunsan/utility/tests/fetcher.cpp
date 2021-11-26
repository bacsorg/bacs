#define BOOST_TEST_MODULE fetcher
#include <boost/test/unit_test.hpp>

#include <bunsan/test/filesystem/read_data.hpp>
#include <bunsan/test/filesystem/tempfiles.hpp>
#include <bunsan/test/filesystem/write_data.hpp>
#include <bunsan/test/test_tools.hpp>

#include "utility_fixture.hpp"

#include <bunsan/utility/fetcher.hpp>

#include <boost/filesystem/operations.hpp>

BOOST_FIXTURE_TEST_SUITE(fetcher, utility_fixture)

BOOST_AUTO_TEST_CASE(copy) {
  const auto ft = bu::fetcher::instance("copy", bu::utility_config{}, resolver);
  bunsan::test::filesystem::tempfiles tmp;
  const auto src = tmp.allocate();
  const auto dst = tmp.allocate();
  bunsan::test::filesystem::write_data(src, "some data");
  ft->fetch(src.string(), dst);
  BOOST_CHECK_EQUAL(bunsan::test::filesystem::read_data(dst), "some data");
}

BOOST_AUTO_TEST_CASE(curl) {
  MOCK_EXPECT(resolver.find_executable).once().with("curl").returns("exe");
  const auto ft = bu::fetcher::instance("curl", bu::utility_config{}, resolver);
  MOCK_EXPECT(executor->sync_execute)
      .calls([](const bunsan::process::context &ctx) {
        BUNSAN_IF_CHECK_EQUAL(ctx.arguments().size(), 6) {
          BOOST_CHECK_EQUAL(ctx.arguments()[0], "exe");
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

BOOST_AUTO_TEST_CASE(wget) {
  MOCK_EXPECT(resolver.find_executable).once().with("wget").returns("exe");
  const auto ft = bu::fetcher::instance("wget", bu::utility_config{}, resolver);
  MOCK_EXPECT(executor->sync_execute)
      .calls([](const bunsan::process::context &ctx) {
        BUNSAN_IF_CHECK_EQUAL(ctx.arguments().size(), 4) {
          BOOST_CHECK_EQUAL(ctx.arguments()[0], "exe");
          BOOST_CHECK_EQUAL(
              ctx.arguments()[1],
              "--output-document=" +
                  (boost::filesystem::current_path() / "dst").string());
          BOOST_CHECK_EQUAL(ctx.arguments()[2], "--quiet");
          BOOST_CHECK_EQUAL(ctx.arguments()[3], "src");
        }
        return 0;
      });
  ft->fetch("src", "dst");
}

BOOST_AUTO_TEST_SUITE_END()  // fetcher
