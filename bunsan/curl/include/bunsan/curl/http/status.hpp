#pragma once

#include <bunsan/curl/http_version.hpp>

#include <string>

namespace bunsan {
namespace curl {
namespace http {

struct status {
  status() = default;
  status(const status &) = default;
  status(status &&) = default;
  status &operator=(const status &) = default;
  status &operator=(status &&) = default;

  status(const http_version version_, const unsigned code_,
         const std::string &reason_)
      : version(version_), code(code_), reason(reason_) {}

  bool operator==(const status &s) const {
    return version == s.version && code == s.code && reason == s.reason;
  }

  http_version version = http_version::http_none;
  unsigned code = 0;
  std::string reason;

  static status parse(const std::string &header);
};

std::ostream &operator<<(std::ostream &out, const status &s);

}  // namespace http
}  // namespace curl
}  // namespace bunsan
