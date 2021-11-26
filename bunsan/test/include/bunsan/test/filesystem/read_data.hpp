#pragma once

#include <boost/filesystem/path.hpp>

namespace bunsan::test::filesystem {

std::string read_data(const boost::filesystem::path &path);

}  // namespace bunsan::test::filesystem
