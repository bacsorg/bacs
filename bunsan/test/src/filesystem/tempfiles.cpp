#include <bunsan/test/filesystem/tempfiles.hpp>
#include <bunsan/test/filesystem/write_data.hpp>

#include <boost/filesystem/operations.hpp>

namespace bunsan {
namespace test {
namespace filesystem {

tempfiles::tempfiles() {}

tempfiles::~tempfiles() {
  for (const boost::filesystem::path &file : m_files)
    boost::filesystem::remove(file);
}

boost::filesystem::path tempfiles::allocate() {
  m_files.push_back(boost::filesystem::temp_directory_path() /
                    boost::filesystem::unique_path());
  write_data(m_files.back(), "");
  return m_files.back();
}

}  // namespace filesystem
}  // namespace test
}  // namespace bunsan
