#define BOOST_TEST_MODULE file
#include <boost/test/unit_test.hpp>

#include <bacs/file.hpp>

BOOST_AUTO_TEST_SUITE(file)

BOOST_AUTO_TEST_SUITE(path)

BOOST_AUTO_TEST_CASE(empty) {
  const boost::filesystem::path path = "";
  const bacs::file::Path proto_path = bacs::file::path_cast(path);
  BOOST_CHECK_EQUAL(proto_path.root(), "");
  BOOST_CHECK_EQUAL(proto_path.element_size(), 0);
  BOOST_CHECK_EQUAL(bacs::file::path_cast(proto_path), path);
}

BOOST_AUTO_TEST_CASE(root) {
  const boost::filesystem::path path = "/";
  const bacs::file::Path proto_path = bacs::file::path_cast(path);
  BOOST_CHECK_EQUAL(proto_path.root(), "/");
  BOOST_CHECK_EQUAL(proto_path.element_size(), 0);
  BOOST_CHECK_EQUAL(bacs::file::path_cast(proto_path), path);
}

BOOST_AUTO_TEST_CASE(absolute) {
  const boost::filesystem::path path = "/root/path";
  const bacs::file::Path proto_path = bacs::file::path_cast(path);
  BOOST_CHECK_EQUAL(proto_path.root(), "/");
  BOOST_CHECK_EQUAL(proto_path.element_size(), 2);
  BOOST_CHECK_EQUAL(proto_path.element(0), "root");
  BOOST_CHECK_EQUAL(proto_path.element(1), "path");
  BOOST_CHECK_EQUAL(bacs::file::path_cast(proto_path), path);
}

BOOST_AUTO_TEST_CASE(relative) {
  const boost::filesystem::path path = "relative/path";
  const bacs::file::Path proto_path = bacs::file::path_cast(path);
  BOOST_CHECK_EQUAL(proto_path.root(), "");
  BOOST_CHECK_EQUAL(proto_path.element_size(), 2);
  BOOST_CHECK_EQUAL(proto_path.element(0), "relative");
  BOOST_CHECK_EQUAL(proto_path.element(1), "path");
  BOOST_CHECK_EQUAL(bacs::file::path_cast(proto_path), path);
}

BOOST_AUTO_TEST_SUITE_END()  // path

BOOST_AUTO_TEST_SUITE_END()  // file
