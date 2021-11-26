#define BOOST_TEST_MODULE filesystem
#include <boost/test/unit_test.hpp>

#include <bunsan/test/filesystem/tempfiles.hpp>
#include <bunsan/test/filesystem/tempfile.hpp>
#include <bunsan/test/filesystem/tempdir.hpp>
#include <bunsan/test/filesystem/read_data.hpp>
#include <bunsan/test/filesystem/write_data.hpp>

#include <boost/filesystem/fstream.hpp>
#include <boost/filesystem/operations.hpp>

BOOST_AUTO_TEST_SUITE(test)

BOOST_AUTO_TEST_SUITE(filesystem)

BOOST_AUTO_TEST_CASE(tempfiles) {
  boost::filesystem::path path1, path2;
  {
    bunsan::test::filesystem::tempfiles tmp;
    path1 = tmp.allocate();
    path2 = tmp.allocate();
    BOOST_REQUIRE(boost::filesystem::exists(path1));
    BOOST_REQUIRE(boost::filesystem::exists(path2));
  }
  BOOST_REQUIRE(!boost::filesystem::exists(path1));
  BOOST_REQUIRE(!boost::filesystem::exists(path2));
}

BOOST_AUTO_TEST_CASE(tempfile) {
  boost::filesystem::path path;
  {
    bunsan::test::filesystem::tempfile tmp;
    path = tmp.path;
    BOOST_REQUIRE(boost::filesystem::exists(path));
  }
  BOOST_REQUIRE(!boost::filesystem::exists(path));
}

BOOST_AUTO_TEST_CASE(tempdir) {
  boost::filesystem::path path;
  {
    bunsan::test::filesystem::tempdir tmp;
    path = tmp.path;
    BOOST_REQUIRE(boost::filesystem::exists(path));
  }
  BOOST_CHECK(!boost::filesystem::exists(path));
  if (boost::filesystem::exists(path))
    BOOST_REQUIRE(boost::filesystem::remove(path));
}

BOOST_FIXTURE_TEST_CASE(read_data, bunsan::test::filesystem::tempdir) {
  const boost::filesystem::path file = path / "file";
  const std::string some_data = "Hello, world! I am test framework.";
  boost::filesystem::ofstream fout(file);
  fout << some_data;
  fout.close();
  BOOST_REQUIRE(boost::filesystem::exists(file));
  BOOST_CHECK_EQUAL(bunsan::test::filesystem::read_data(file), some_data);
}

BOOST_FIXTURE_TEST_CASE(write_data, bunsan::test::filesystem::tempdir) {
  const boost::filesystem::path file = path / "file";
  const std::string some_data = "Hello, world! I am test framework.";
  bunsan::test::filesystem::write_data(file, some_data);
  BOOST_CHECK_EQUAL(bunsan::test::filesystem::read_data(file), some_data);
}

BOOST_AUTO_TEST_SUITE_END()  // filesystem

BOOST_AUTO_TEST_SUITE_END()  // test
