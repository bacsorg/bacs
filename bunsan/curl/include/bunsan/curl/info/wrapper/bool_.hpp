#pragma once

#include <bunsan/curl/error.hpp>
#include <bunsan/curl/info/wrapper/basic_wrapper.hpp>
#include <bunsan/curl/info/wrapper/primitive.hpp>

#include <curl/curl.h>

namespace bunsan::curl::info::wrapper {

template <CURLINFO Info>
struct bool_ : basic_wrapper<bool, Info> {
  static bool getinfo(CURL *easy) { return long_<Info>::getinfo(easy); }
};

}  // namespace bunsan::curl::info::wrapper
