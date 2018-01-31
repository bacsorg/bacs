#pragma once

#include <bunsan/curl/http/header_set.hpp>
#include <bunsan/curl/http/status.hpp>

#include <ostream>
#include <string>

namespace bunsan {
namespace curl {
namespace http {

class response_head {
 public:
  explicit response_head(const http::status &s);

  response_head(const response_head &) = default;
  response_head(response_head &&) = default;
  response_head &operator=(const response_head &) = default;
  response_head &operator=(response_head &&) = default;

  void append(const header &h) { m_header_set.merge_insert(h); }

  const http::status &status() const { return m_status; }
  const header_set &headers() const { return m_header_set; }

  bool operator==(const response_head &h) const {
    return m_status == h.m_status && m_header_set == h.m_header_set;
  }

 private:
  http::status m_status;
  header_set m_header_set;
};

std::ostream &operator<<(std::ostream &out, const response_head &r);

}  // namespace http
}  // namespace curl
}  // namespace bunsan
