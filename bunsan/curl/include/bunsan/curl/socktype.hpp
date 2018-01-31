#pragma once

#include <curl/curl.h>

#include <bunsan/stream_enum.hpp>

namespace bunsan {
namespace curl {

BUNSAN_TYPED_STREAM_ENUM_CLASS_INITIALIZED(socktype, long, (
  (ipcxn, CURLSOCKTYPE_IPCXN),
  (accept, CURLSOCKTYPE_ACCEPT)
))
static_assert(static_cast<long>(socktype::accept) + 1 == CURLSOCKTYPE_LAST,
              "outdated");

}  // namespace curl
}  // namespace bunsan
