#include <bacs/file.hpp>

namespace bacs {
namespace file {

Path path_cast(const boost::filesystem::path &path) {
  Path tmp;
  path_convert(path, tmp);
  return tmp;
}

boost::filesystem::path path_cast(const Path &proto_path) {
  boost::filesystem::path path;
  path_convert(proto_path, path);
  return path;
}

void path_convert(const boost::filesystem::path &path, Path &proto_path) {
  proto_path.Clear();
  proto_path.set_root(path.root_directory().string());
  for (const boost::filesystem::path &r : path.relative_path()) {
    *proto_path.add_element() = r.string();
  }
}

void path_convert(const Path &proto_path, boost::filesystem::path &path) {
  path = proto_path.root();
  for (const std::string &element : proto_path.element()) path /= element;
}

}  // namespace file
}  // namespace bacs
