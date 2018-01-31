#pragma once

#include <curl/curl.h>

#include <bunsan/stream_enum.hpp>

namespace bunsan {
namespace curl {

BUNSAN_TYPED_STREAM_ENUM_CLASS_INITIALIZED(fnmatch, int, (
  (match, CURL_FNMATCHFUNC_MATCH),
  (nomatch, CURL_FNMATCHFUNC_NOMATCH),
  (fail, CURL_FNMATCHFUNC_FAIL)
))

}  // namespace curl
}  // namespace bunsan
