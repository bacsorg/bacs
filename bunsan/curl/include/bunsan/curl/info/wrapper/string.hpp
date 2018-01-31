#pragma once

#include <bunsan/curl/error.hpp>
#include <bunsan/curl/info/wrapper/basic_wrapper.hpp>
#include <bunsan/curl/info/wrapper/primitive.hpp>

#include <curl/curl.h>

namespace bunsan {
namespace curl {
namespace info {
namespace wrapper {

struct nullptr_info_error : virtual error {};

template <CURLINFO Info>
struct string : basic_wrapper<std::string, Info> {
  static std::string getinfo(CURL *easy) {
    const auto str = char_ptr<Info>::getinfo(easy);
    if (!str) BOOST_THROW_EXCEPTION(nullptr_info_error());
    return std::string(str);
  }
};

}  // namespace wrapper
}  // namespace info
}  // namespace curl
}  // namespace bunsan
