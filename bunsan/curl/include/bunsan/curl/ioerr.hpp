#pragma once

#include <curl/curl.h>

#include <bunsan/stream_enum.hpp>

namespace bunsan {
namespace curl {

BUNSAN_TYPED_STREAM_ENUM_CLASS_INITIALIZED(ioerr, long, (
  (ok, CURLIOE_OK),
  (unknowncmd, CURLIOE_UNKNOWNCMD),
  (failrestart, CURLIOE_FAILRESTART)
))
static_assert(static_cast<long>(ioerr::failrestart) + 1 == CURLIOE_LAST,
              "outdated");

}  // namespace curl
}  // namespace bunsan
