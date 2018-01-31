#define BOOST_TEST_MODULE tempfile
#include <boost/test/unit_test.hpp>

#include <bunsan/tempfile.hpp>

#include <boost/assert.hpp>
#include <boost/filesystem.hpp>
#include <boost/filesystem/fstream.hpp>

BOOST_AUTO_TEST_SUITE(tempfile)

using bunsan::tempfile;
using boost::filesystem::path;

const path &getpath(const tempfile &tmp) { return tmp.path(); }

const path &getpath(const path &path_) { return path_; }

template <typename T>
bool exists(const T &file) {
  return boost::filesystem::exists(getpath(file));
}

template <typename T>
void touch(const T &file) {
  boost::filesystem::ofstream tmp(getpath(file));
  tmp.close();
  BOOST_REQUIRE(tmp);
  BOOST_REQUIRE(exists(file));
}

BOOST_AUTO_TEST_CASE(auto_remove) {
  boost::filesystem::path tmppath;
  {
    tempfile tmp = tempfile::regular_file_in_tempdir();
    tmppath = tmp.path();
    BOOST_CHECK(tmp.auto_remove());
    BOOST_CHECK(exists(tmp));
  }
  BOOST_CHECK(!exists(tmppath));
}

BOOST_AUTO_TEST_CASE(swap_) {
  boost::filesystem::path tmppath;
  tempfile tmp = tempfile::regular_file_in_tempdir();
  tmppath = tmp.path();
  tempfile tmp2;
  BOOST_CHECK(tmp.auto_remove());
  BOOST_CHECK(!tmp2.auto_remove());
  swap(tmp, tmp2);
  BOOST_CHECK(!tmp.auto_remove());
  BOOST_CHECK(tmp2.auto_remove());
  BOOST_CHECK(tmp2.path() == tmppath);
}

BOOST_AUTO_TEST_CASE(model) {
  tempfile tmp = tempfile::regular_file_in_tempdir("%%%%-%%%%-%%%%-%%%%.tgz");
  BOOST_CHECK_EQUAL(tmp.path().extension(), ".tgz");
}

BOOST_AUTO_TEST_SUITE(create)

BOOST_AUTO_TEST_CASE(regular_file_fail) {
  BOOST_CHECK_THROW((tempfile::regular_file_in_tempdir("no-percent-string"),
                     tempfile::regular_file_in_tempdir("no-percent-string")),
                    bunsan::unable_to_create_unique_temp_regular_file);
}

BOOST_AUTO_TEST_CASE(regular_file) {
  const tempfile tmp =
      tempfile::regular_file_in_tempdir("%%%%-%%%%-%%%%-%%%%.suff");
  BOOST_CHECK(boost::filesystem::is_regular_file(tmp.path()));
  BOOST_CHECK_EQUAL(tmp.path().extension(), ".suff");
}

BOOST_AUTO_TEST_CASE(directory_file_fail) {
  BOOST_CHECK_THROW((tempfile::directory_in_tempdir("no-percent-string"),
                     tempfile::directory_in_tempdir("no-percent-string")),
                    bunsan::unable_to_create_unique_temp_directory);
}

BOOST_AUTO_TEST_CASE(directory) {
  const tempfile tmp =
      tempfile::directory_in_tempdir("%%%%%-%%%%%-%%%%-%%%%.suff");
  BOOST_CHECK(boost::filesystem::is_directory(tmp.path()));
  BOOST_CHECK_EQUAL(tmp.path().extension(), ".suff");
}

BOOST_AUTO_TEST_SUITE_END()  // create

BOOST_AUTO_TEST_SUITE_END()  // tempfile
