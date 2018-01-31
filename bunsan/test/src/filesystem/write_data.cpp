#include <bunsan/test/filesystem/write_data.hpp>

#include <boost/filesystem/operations.hpp>
#include <boost/filesystem/fstream.hpp>
#include <boost/test/unit_test.hpp>

namespace bunsan {
namespace test {
namespace filesystem {

void write_data(const boost::filesystem::path &path, const std::string &data) {
  BOOST_TEST_CHECKPOINT(BOOST_CURRENT_FUNCTION);
  BOOST_TEST_MESSAGE("Attempt to write to " << path << ".");
  boost::filesystem::ofstream fout(path);
  BOOST_REQUIRE(fout.is_open());
  fout << data << std::flush;
  BOOST_REQUIRE(fout);
  fout.close();
  BOOST_REQUIRE(fout);
}

}  // namespace filesystem
}  // namespace test
}  // namespace bunsan
