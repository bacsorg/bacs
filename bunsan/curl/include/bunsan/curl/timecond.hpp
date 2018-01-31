#pragma once

#include <curl/curl.h>

#include <bunsan/stream_enum.hpp>

namespace bunsan {
namespace curl {

BUNSAN_TYPED_STREAM_ENUM_CLASS_INITIALIZED(timecond, long, (
  (none, CURL_TIMECOND_NONE),
  (ifmodsince, CURL_TIMECOND_IFMODSINCE),
  (ifunmodsince, CURL_TIMECOND_IFUNMODSINCE),
  (lastmod, CURL_TIMECOND_LASTMOD)
))
static_assert(static_cast<long>(timecond::lastmod) + 1 == CURL_TIMECOND_LAST,
              "outdated");

}  // namespace curl
}  // namespace bunsan
