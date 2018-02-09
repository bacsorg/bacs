#pragma once

#include <boost/filesystem/path.hpp>

namespace bunsan::test::filesystem {

void write_data(const boost::filesystem::path &path, const std::string &data);

}  // namespace bunsan::test::filesystem
