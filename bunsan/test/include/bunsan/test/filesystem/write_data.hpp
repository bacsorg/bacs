#pragma once

#include <boost/filesystem/path.hpp>

namespace bunsan {
namespace test {
namespace filesystem {

void write_data(const boost::filesystem::path &path, const std::string &data);

}  // namespace filesystem
}  // namespace test
}  // namespace bunsan
