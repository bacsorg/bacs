#include <bunsan/test/filesystem/tempdir.hpp>

#include <boost/filesystem/operations.hpp>
#include <boost/test/test_tools.hpp>

namespace bunsan {
namespace test {
namespace filesystem {

tempdir::tempdir()
    : path(boost::filesystem::temp_directory_path() /
           boost::filesystem::unique_path()) {
  BOOST_REQUIRE(boost::filesystem::create_directory(path));
  BOOST_TEST_MESSAGE("Temporary directory " << path << " was created.");
}

tempdir::~tempdir() {
  boost::system::error_code ec;
  boost::filesystem::remove_all(path, ec);
  BOOST_CHECK(!ec);
  if (ec)
    BOOST_TEST_MESSAGE("Error while removing temporary directory "
                       << path << ": " << ec.message() << ".");
}

}  // namespace filesystem
}  // namespace test
}  // namespace bunsan
