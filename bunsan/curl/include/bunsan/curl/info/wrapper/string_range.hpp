#pragma once

#include <bunsan/curl/detail/slist_iterator.hpp>
#include <bunsan/curl/error.hpp>
#include <bunsan/curl/info/wrapper/basic_wrapper.hpp>
#include <bunsan/curl/info/wrapper/primitive.hpp>

#include <curl/curl.h>

namespace bunsan {
namespace curl {
namespace info {
namespace wrapper {

template <CURLINFO Info>
struct string_range : basic_wrapper<detail::slist_string_range, Info> {
  static detail::slist_string_range getinfo(CURL *easy) {
    const auto list = slist_ptr<Info>::getinfo(easy);
    return detail::make_slist_string_range(list);
  }
};

}  // namespace wrapper
}  // namespace info
}  // namespace curl
}  // namespace bunsan
