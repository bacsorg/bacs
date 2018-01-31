#pragma once

#include <curl/curl.h>

#include <bunsan/stream_enum.hpp>

namespace bunsan {
namespace curl {

BUNSAN_TYPED_STREAM_ENUM_CLASS_INITIALIZED(ipresolve, long, (
  (whatever, CURL_IPRESOLVE_WHATEVER),
  (v4, CURL_IPRESOLVE_V4),
  (v6, CURL_IPRESOLVE_V6)
))

}  // namespace curl
}  // namespace bunsan
