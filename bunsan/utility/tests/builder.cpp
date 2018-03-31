#define BOOST_TEST_MODULE builder
#include <boost/test/unit_test.hpp>

#include <bunsan/test/filesystem/tempfile.hpp>
#include <bunsan/test/test_tools.hpp>

#include "utility_fixture.hpp"

#include <bunsan/utility/builder.hpp>

#include <boost/filesystem/operations.hpp>

BOOST_FIXTURE_TEST_SUITE(builder, utility_fixture)

BOOST_AUTO_TEST_CASE(cmake) {
  MOCK_EXPECT(resolver.find_executable)
      .once()
      .with("cmake")
      .returns("cmake-exe");
  MOCK_EXPECT(resolver.find_executable).once().with("make").returns("make-exe");
  bu::utility_config cfg;
  cfg.put("cmake.defines.FOO", "bar");
  cfg.put("cmake.generator", "Fake Makefiles");
  cfg.put("maker.jobs", 3);
  const auto br = bu::builder::instance("cmake", cfg, resolver);
  MOCK_EXPECT(executor->sync_execute)
      .once()
      .calls([](const bunsan::process::context &ctx) {
        BOOST_CHECK_EQUAL(ctx.current_path(), "/some/bin");
        BUNSAN_IF_CHECK_EQUAL(ctx.arguments().size(), 5) {
          BOOST_CHECK_EQUAL(ctx.arguments()[0], "cmake-exe");
          BOOST_CHECK_EQUAL(ctx.arguments()[1], "-G");
          BOOST_CHECK_EQUAL(ctx.arguments()[2], "Fake Makefiles");
          BOOST_CHECK_EQUAL(ctx.arguments()[3], "-DFOO=bar");
          BOOST_CHECK_EQUAL(ctx.arguments()[4], "/foo/src");
        }
        return 0;
      });
  MOCK_EXPECT(executor->sync_execute)
      .once()
      .calls([](const bunsan::process::context &ctx) {
        BOOST_CHECK_EQUAL(ctx.current_path(), "/some/bin");
        BUNSAN_IF_CHECK_EQUAL(ctx.arguments().size(), 2) {
          BOOST_CHECK_EQUAL(ctx.arguments()[0], "make-exe");
          BOOST_CHECK_EQUAL(ctx.arguments()[1], "-j3");
        }
        return 0;
      });
  MOCK_EXPECT(executor->sync_execute)
      .once()
      .calls([](const bunsan::process::context &ctx) {
        BOOST_CHECK_EQUAL(ctx.current_path(), "/some/bin");
        BUNSAN_IF_CHECK_EQUAL(ctx.arguments().size(), 4) {
          BOOST_CHECK_EQUAL(ctx.arguments()[0], "make-exe");
          BOOST_CHECK_EQUAL(ctx.arguments()[1], "DESTDIR=/usr/root");
          BOOST_CHECK_EQUAL(ctx.arguments()[2], "-j3");
          BOOST_CHECK_EQUAL(ctx.arguments()[3], "install");
        }
        return 0;
      });
  br->install("/foo/src", "/some/bin", "/usr/root");
}

BOOST_AUTO_TEST_SUITE_END()  // builder
