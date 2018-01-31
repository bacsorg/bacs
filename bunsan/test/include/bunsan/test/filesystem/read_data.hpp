#pragma once

#include <boost/filesystem/path.hpp>

namespace bunsan {
namespace test {
namespace filesystem {

std::string read_data(const boost::filesystem::path &path);

}  // namespace filesystem
}  // namespace test
}  // namespace bunsan
