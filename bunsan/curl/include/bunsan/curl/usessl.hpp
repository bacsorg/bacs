#pragma once

#include <curl/curl.h>

#include <bunsan/stream_enum.hpp>

namespace bunsan {
namespace curl {

BUNSAN_TYPED_STREAM_ENUM_CLASS_INITIALIZED(usessl, long, (
  (none, CURLUSESSL_NONE),
  (try_, CURLUSESSL_TRY),
  (control, CURLUSESSL_CONTROL),
  (all, CURLUSESSL_ALL)
))
static_assert(static_cast<long>(usessl::all) + 1 == CURLUSESSL_LAST,
              "outdated");

}  // namespace curl
}  // namespace bunsan
