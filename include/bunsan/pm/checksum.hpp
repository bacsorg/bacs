#pragma once

#include <boost/filesystem/path.hpp>

#include <string>

namespace bunsan {
namespace pm {

std::string checksum(const boost::filesystem::path &file);

}  // namespace pm
}  // namespace bunsan
