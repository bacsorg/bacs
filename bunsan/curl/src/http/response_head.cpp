#include <bunsan/curl/http/response_head.hpp>

namespace bunsan::curl::http {

response_head::response_head(const http::status &s) : m_status(s) {}

std::ostream &operator<<(std::ostream &out, const response_head &r) {
  out << r.status() << '\n';
  out << r.headers() << '\n';
  return out;
}

}  // namespace bunsan::curl::http
