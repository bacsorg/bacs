#pragma once

#include <bacs/file/path.pb.h>

#include <boost/filesystem/path.hpp>

namespace bacs {
namespace file {

Path path_cast(const boost::filesystem::path &path);
boost::filesystem::path path_cast(const Path &proto_path);

void path_convert(const boost::filesystem::path &path, Path &proto_path);
void path_convert(const Path &proto_path, boost::filesystem::path &path);

}  // namespace file
}  // namespace bacs
