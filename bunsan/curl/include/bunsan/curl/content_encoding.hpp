#pragma once

#include <curl/curl.h>

#include <bunsan/stream_enum.hpp>

namespace bunsan {
namespace curl {

BUNSAN_STREAM_ENUM_CLASS(content_encoding, (identity, deflate, gzip))

}  // namespace curl
}  // namespace bunsan
