#define BOOST_TEST_MODULE execute
#include <boost/test/unit_test.hpp>

#include <bunsan/test/filesystem/write_data.hpp>
#include <bunsan/test/test_tools.hpp>

#include <bunsan/process/execute.hpp>
#include <bunsan/process/executor.hpp>
#include <bunsan/process/mock_executor.hpp>

namespace bp = bunsan::process;

BOOST_FIXTURE_TEST_SUITE(execute, bp::mock_executor_fixture)

BOOST_AUTO_TEST_CASE(sync_execute) {
  MOCK_EXPECT(executor->sync_execute)
      .once()
      .with([](const bp::context &ctx) {
        BUNSAN_IF_CHECK_EQUAL(ctx.arguments().size(), 2) {
          BOOST_CHECK_EQUAL(ctx.arguments()[0], "hello");
          BOOST_CHECK_EQUAL(ctx.arguments()[1], "world");
        }
        return true;
      })
      .returns(0);
  bp::sync_execute("hello", "world");
}

BOOST_AUTO_TEST_CASE(check_sync_execute) {
  MOCK_EXPECT(executor->sync_execute).once().with(mock::any).returns(0);
  MOCK_EXPECT(executor->sync_execute).once().with(mock::any).returns(1);
  BOOST_CHECK_NO_THROW(bp::check_sync_execute("hello"));
  BOOST_CHECK_THROW(bp::check_sync_execute("hello"),
                    bp::non_zero_exit_status_error);
}

BOOST_AUTO_TEST_CASE(sync_execute_with_output) {
  MOCK_EXPECT(executor->sync_execute)
      .once()
      .calls([](const bp::context &ctx) {
        BUNSAN_IF_CHECK_EQUAL(ctx.arguments().size(), 2) {
          BOOST_CHECK_EQUAL(ctx.arguments()[0], "hello");
          BOOST_CHECK_EQUAL(ctx.arguments()[1], "world");
        }
        const auto out =
            boost::get<boost::filesystem::path>(&ctx.stdout_data());
        BUNSAN_IF_CHECK(out) {
          bunsan::test::filesystem::write_data(*out, "some data");
        }
        BOOST_CHECK(
            boost::get<bp::file::redirect_to_stdout_type>(&ctx.stderr_data()));
        return 10;
      });
  std::string output;
  BOOST_CHECK_EQUAL(bp::sync_execute_with_output(
                        bp::context().arguments("hello", "world"), output),
                    10);
  BOOST_CHECK_EQUAL(output, "some data");
}

BOOST_AUTO_TEST_SUITE_END()  // execute
