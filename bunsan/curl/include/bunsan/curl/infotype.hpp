#pragma once

#include <curl/curl.h>

#include <bunsan/stream_enum.hpp>

namespace bunsan {
namespace curl {

BUNSAN_TYPED_STREAM_ENUM_CLASS_INITIALIZED(infotype, long, (
  (text, CURLINFO_TEXT),
  (header_in, CURLINFO_HEADER_IN),
  (header_out, CURLINFO_HEADER_OUT),
  (data_in, CURLINFO_DATA_IN),
  (data_out, CURLINFO_DATA_OUT),
  (ssl_data_in, CURLINFO_SSL_DATA_IN),
  (ssl_data_out, CURLINFO_SSL_DATA_OUT)
))
static_assert(static_cast<long>(infotype::ssl_data_out) + 1 == CURLINFO_END,
              "outdated");

}  // namespace curl
}  // namespace bunsan
