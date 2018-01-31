#define BOOST_TEST_MODULE file_guard
#include <boost/test/unit_test.hpp>

#include <bunsan/filesystem/error.hpp>
#include <bunsan/interprocess/sync/file_guard.hpp>
#include <bunsan/test/filesystem/tempdir.hpp>

#include <boost/filesystem/operations.hpp>

struct file_guard_fixture {
  const boost::filesystem::path lock_path;

  file_guard_fixture()
      : lock_path(boost::filesystem::temp_directory_path() /
                  boost::filesystem::unique_path()) {
    BOOST_REQUIRE(!boost::filesystem::exists(lock_path));
  }

  ~file_guard_fixture() {
    BOOST_REQUIRE(!boost::filesystem::exists(lock_path));
  }
};

BOOST_FIXTURE_TEST_SUITE(file_guard, file_guard_fixture)

using bunsan::interprocess::file_guard;

BOOST_AUTO_TEST_CASE(ctor) {
  file_guard fg;
  BOOST_CHECK(!fg);
  file_guard fg2(lock_path);
  BOOST_CHECK(fg2);
  BOOST_CHECK(boost::filesystem::exists(lock_path));
  // move it
  fg = std::move(fg2);
  BOOST_CHECK(fg);
  BOOST_CHECK(!fg2);
  // remote it
  fg.remove();
  BOOST_CHECK(!fg);
  BOOST_CHECK(!fg2);
  BOOST_CHECK(!boost::filesystem::exists(lock_path));
}

BOOST_AUTO_TEST_CASE(collision) {
  file_guard fg(lock_path);
  BOOST_CHECK(fg);
  BOOST_CHECK_THROW(file_guard new_fg(lock_path),
                    bunsan::interprocess::file_guard_locked_error);
  fg.remove();
  BOOST_CHECK(!fg);
  file_guard new_fg(lock_path);
  BOOST_CHECK(new_fg);
}

BOOST_AUTO_TEST_CASE(directory_lock) {
  BOOST_REQUIRE(boost::filesystem::create_directory(lock_path));
  BOOST_CHECK_THROW(file_guard fg(lock_path),
                    bunsan::interprocess::file_guard_locked_error);
  BOOST_REQUIRE(boost::filesystem::remove(lock_path));
}

BOOST_AUTO_TEST_CASE(permission_denied) {
  // create directory with no write permissions
  const bunsan::test::filesystem::tempdir tmpdir;
  const boost::filesystem::perms all_read = boost::filesystem::owner_read |
                                            boost::filesystem::group_read |
                                            boost::filesystem::others_read;
  const boost::filesystem::perms all_exe = boost::filesystem::owner_exe |
                                           boost::filesystem::group_exe |
                                           boost::filesystem::others_exe;
  boost::filesystem::permissions(tmpdir.path, all_read | all_exe);
  const boost::filesystem::path &no_perm_dir = tmpdir.path;
  const boost::filesystem::path no_perm_file =
      no_perm_dir / boost::filesystem::unique_path();
  BOOST_REQUIRE(boost::filesystem::exists(no_perm_dir));
  BOOST_REQUIRE(!boost::filesystem::exists(no_perm_file));
  const auto check_system_error =
      [&](const bunsan::filesystem::system_error &e) {
        const boost::filesystem::path *const path =
            e.get<bunsan::filesystem::system_error::path>();
        return path && *path == no_perm_file;
      };
  const auto check_create_error = [&](
      const bunsan::interprocess::file_guard_create_error &e) {
    if (e.get<bunsan::error::nested_exception>()) {
      BOOST_CHECK_EXCEPTION(
          boost::rethrow_exception(*e.get<bunsan::error::nested_exception>()),
          bunsan::filesystem::system_error, check_system_error);
      return true;
    }
    return false;
  };
  BOOST_CHECK_EXCEPTION(file_guard fg(no_perm_file),
                        bunsan::interprocess::file_guard_create_error,
                        check_create_error);
}

BOOST_AUTO_TEST_SUITE_END()  // file_guard
