#pragma once

#include <curl/curl.h>

#include <bunsan/stream_enum.hpp>

namespace bunsan {
namespace curl {

BUNSAN_TYPED_STREAM_ENUM_CLASS_INITIALIZED(http_version, long, (
  (http_none, CURL_HTTP_VERSION_NONE),
  (http_1_0, CURL_HTTP_VERSION_1_0),
  (http_1_1, CURL_HTTP_VERSION_1_1),
  (http_2_0, CURL_HTTP_VERSION_2_0),
  (http_2tls, CURL_HTTP_VERSION_2TLS),
  (http_2_prior_knowledge, CURL_HTTP_VERSION_2_PRIOR_KNOWLEDGE)
))
static_assert(static_cast<long>(http_version::http_2_prior_knowledge) + 1 ==
                  CURL_HTTP_VERSION_LAST,
              "outdated");

struct http_version_unsupported_error
    : virtual http_version_invalid_value_error {
  using version_major = boost::error_info<struct tag_version_major, unsigned>;
  using version_minor = boost::error_info<struct tag_version_minor, unsigned>;
};

struct http_version_pair {
  http_version_pair() = default;
  http_version_pair(const http_version_pair &) = default;
  http_version_pair(http_version_pair &&) = default;
  http_version_pair &operator=(const http_version_pair &) = default;
  http_version_pair &operator=(http_version_pair &&) = default;

  // if version == http_none creates default
  explicit http_version_pair(const http_version version);

  constexpr http_version_pair(const unsigned major_, const unsigned minor_)
      : version_major(major_), version_minor(minor_) {}

  /* implicit */ operator http_version() const;

  bool operator==(const http_version_pair &v) const {
    return version_major == v.version_major && version_minor == v.version_minor;
  }

  unsigned version_major = 0;
  unsigned version_minor = 0;
};

http_version_pair make_http_version_pair(const http_version version);

std::ostream &operator<<(std::ostream &out, const http_version_pair &version);

}  // namespace curl
}  // namespace bunsan
