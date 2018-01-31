#define BOOST_TEST_MODULE process
#include <boost/test/unit_test.hpp>

#include <bunsan/test/exec_test.hpp>
#include <bunsan/test/filesystem/read_data.hpp>
#include <bunsan/test/filesystem/tempfile.hpp>
#include <bunsan/test/filesystem/tempfiles.hpp>
#include <bunsan/test/filesystem/write_data.hpp>

#include <bunsan/process/execute.hpp>

#include <stdlib.h>  // setenv(), unsetenv()

namespace bp = bunsan::process;

BOOST_AUTO_TEST_SUITE(process)

BOOST_AUTO_TEST_SUITE(unix)

BOOST_FIXTURE_TEST_SUITE(sync_execute, bunsan::test::filesystem::tempfile)

BOOST_AUTO_TEST_CASE(exit_status) {
  BOOST_CHECK_EQUAL(bp::sync_execute(bp::context().executable("sh").arguments(
                        "sh", "-c", "exit 10")),
                    10);

  BOOST_CHECK_EQUAL(bp::sync_execute(bp::context().executable("sh").arguments(
                        "sh", "-c", "kill -10 $$")),
                    -10);
}

BOOST_AUTO_TEST_CASE(stdin_data) {
  BOOST_CHECK_EQUAL(
      bp::sync_execute(bp::context().executable("sh").stdin_data("exit 11")),
      11);

  bunsan::test::filesystem::write_data(path, "exit 12");
  BOOST_CHECK_EQUAL(
      bp::sync_execute(bp::context().executable("sh").stdin_file(path)), 12);

  BOOST_CHECK_EQUAL(
      bp::sync_execute(bp::context()
                           .executable("sh")
                           .arguments("sh", "-c", "read x || exit 15")
                           .stdin_suppress()),
      15);
}

BOOST_AUTO_TEST_CASE(stdin_file) {
  bunsan::test::filesystem::write_data(path, "kill -13 $$");
  BOOST_CHECK_EQUAL(
      bp::sync_execute(bp::context().executable("sh").stdin_file(path)), -13);

  BOOST_CHECK_EQUAL(
      bp::sync_execute(bp::context().executable("sh").stdin_file(path)), -13);
}

BOOST_AUTO_TEST_CASE(stdout_file) {
  BOOST_CHECK_EQUAL(bp::sync_execute(bp::context()
                                         .executable("sh")
                                         .stdin_data("echo \"Hello, world\"")
                                         .stdout_file(path)),
                    0);
  BOOST_CHECK_EQUAL(bunsan::test::filesystem::read_data(path),
                    "Hello, world\n");
}

BOOST_AUTO_TEST_CASE(stderr_file) {
  BOOST_CHECK_EQUAL(
      bp::sync_execute(bp::context()
                           .executable("sh")
                           .stdin_data("echo \"Hello, world\" >&2")
                           .stderr_file(path)),
      0);
  BOOST_CHECK_EQUAL(bunsan::test::filesystem::read_data(path),
                    "Hello, world\n");

  BOOST_CHECK_EQUAL(
      bp::sync_execute(bp::context()
                           .executable("sh")
                           .stdin_data("echo \"Hello, world\" >&2")
                           .stdout_file(path)
                           .stderr_redirect_to_stdout()),
      0);
  BOOST_CHECK_EQUAL(bunsan::test::filesystem::read_data(path),
                    "Hello, world\n");
}

BOOST_FIXTURE_TEST_CASE(std_out_err_file, bunsan::test::filesystem::tempfiles) {
  const auto path_out = allocate(), path_err = allocate();
  BOOST_CHECK_EQUAL(bp::sync_execute(bp::context()
                                         .executable("sh")
                                         .stdin_data(
                                              "echo \"Hello, out world\"\n"
                                              "echo \"Hello, err world\" >&2")
                                         .stdout_file(path_out)
                                         .stderr_file(path_err)),
                    0);
  BOOST_CHECK_EQUAL(bunsan::test::filesystem::read_data(path_out),
                    "Hello, out world\n");
  BOOST_CHECK_EQUAL(bunsan::test::filesystem::read_data(path_err),
                    "Hello, err world\n");
}

BOOST_AUTO_TEST_CASE(use_path) {
  BOOST_CHECK_EQUAL(
      bp::sync_execute(bp::context().executable("/bin/sh").use_path(true)), 0);
}

BOOST_AUTO_TEST_CASE(with_output) {
  std::string output;
  BOOST_CHECK_EQUAL(
      bp::sync_execute_with_output(
          bp::context().executable("sh").stdin_data("echo \"Hello, world\""),
          output),
      0);
  BOOST_CHECK_EQUAL(output, "Hello, world");

  BOOST_REQUIRE(!getenv("BUNSAN_PROCESS_INHERIT"));
  BOOST_CHECK_EXCEPTION(bp::check_sync_execute_with_output(
                            bp::context().executable("sh").stdin_data(
                                "echo \"Hello, world\" && exit 1")),
                        bp::check_sync_execute_error,
                        [](const bp::check_sync_execute_error &e) {
                          const std::string *const output =
                              e.get<bp::check_sync_execute_error::output>();
                          return output && *output == "Hello, world";
                        });
}

struct bunsan_process_inherit_setter {
  bunsan_process_inherit_setter() {
    BOOST_REQUIRE_EQUAL(setenv("BUNSAN_PROCESS_INHERIT", "1", 1), 0);
  }

  ~bunsan_process_inherit_setter() {
    BOOST_REQUIRE_EQUAL(unsetenv("BUNSAN_PROCESS_INHERIT"), 0);
  }
};

BOOST_FIXTURE_TEST_CASE(with_output_inherit, bunsan_process_inherit_setter) {
  BOOST_CHECK_EXCEPTION(bp::check_sync_execute_with_output(
                            bp::context().executable("sh").stdin_data(
                                "echo \"Hello, world\" && exit 1")),
                        bp::check_sync_execute_error,
                        [](const bp::check_sync_execute_error &e) {
                          const std::string *const output =
                              e.get<bp::check_sync_execute_error::output>();
                          return !output;
                        });
}

BOOST_AUTO_TEST_SUITE_END()  // sync_execute

BOOST_AUTO_TEST_SUITE_END()  // unix

BOOST_AUTO_TEST_SUITE_END()  // process
