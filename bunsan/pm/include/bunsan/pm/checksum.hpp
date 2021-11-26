#pragma once

#include <boost/filesystem/path.hpp>

#include <string>

namespace bunsan::pm {

std::string checksum(const boost::filesystem::path &file);

}  // namespace bunsan::pm
