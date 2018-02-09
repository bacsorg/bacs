#pragma once

#include <curl/curl.h>

#include <bunsan/stream_enum.hpp>

namespace bunsan::curl {

BUNSAN_TYPED_STREAM_ENUM_CLASS_INITIALIZED(seekfunc, long, (
  (ok, CURL_SEEKFUNC_OK),
  (fail, CURL_SEEKFUNC_FAIL),
  (cantseek, CURL_SEEKFUNC_CANTSEEK)
))

}  // namespace bunsan::curl
