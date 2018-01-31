#pragma once

#include <bunsan/curl/info/wrapper/basic_wrapper.hpp>
#include <bunsan/curl/info/wrapper/primitive.hpp>

#include <curl/curl.h>

#include <boost/optional.hpp>

namespace bunsan {
namespace curl {
namespace info {
namespace wrapper {

template <CURLINFO Info>
struct optional_string : basic_wrapper<boost::optional<std::string>, Info> {
  static boost::optional<std::string> getinfo(CURL *easy) {
    const auto str = char_ptr<Info>::getinfo(easy);
    if (!str) return boost::none;
    return std::string(str);
  }
};

}  // namespace wrapper
}  // namespace info
}  // namespace curl
}  // namespace bunsan
