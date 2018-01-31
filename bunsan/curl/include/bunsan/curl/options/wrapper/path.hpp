#pragma once

#include <bunsan/curl/options/wrapper/string.hpp>

#include <boost/filesystem/path.hpp>

namespace bunsan {
namespace curl {
namespace options {
namespace wrapper {

class path : public string {
 public:
  path() = default;

  explicit path(boost::none_t) : string(boost::none) {}

  template <typename... Args>
  explicit path(Args &&... args)
      : string(boost::filesystem::path(std::forward<Args>(args)...).string()) {}

  explicit path(const boost::filesystem::path &path) : string(path.string()) {}
};

}  // namespace wrapper
}  // namespace options
}  // namespace curl
}  // namespace bunsan
