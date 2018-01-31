#include <bunsan/curl/http/header_parser.hpp>

#include <bunsan/curl/http/error.hpp>

#include <boost/algorithm/string/predicate.hpp>
#include <boost/assert.hpp>

namespace bunsan {
namespace curl {
namespace http {

header_parser::callback_type header_parser::callback() {
  return std::bind(&header_parser::parse_header_callback, this,
                   std::placeholders::_1, std::placeholders::_2);
}

header_parser::option_type header_parser::option() {
  return options::headerfunction(callback());
}

const response_head &header_parser::at(const std::size_t index) const {
  BOOST_ASSERT(index < m_responses.size());
  return m_responses[index];
}

const response_head &header_parser::front() const {
  BOOST_ASSERT(!m_responses.empty());
  return m_responses.front();
}

const response_head &header_parser::back() const {
  BOOST_ASSERT(!m_responses.empty());
  return m_responses.back();
}

void header_parser::parse(const std::string &header_data) {
  if (boost::algorithm::starts_with(header_data, "HTTP/")) {
    m_responses.emplace_back(http::status::parse(header_data));
  } else if (!header_data.empty()) {
    if (m_responses.empty())
      BOOST_THROW_EXCEPTION(status_header_expected_error());
    m_responses.back().append(header::parse(header_data));
  } else {
    // skips
  }
}

std::size_t header_parser::parse_header_callback(const char *ptr,
                                                 std::size_t size) {
  std::string header_data(ptr, size);
  if (!boost::algorithm::ends_with(header_data, "\r\n"))
    BOOST_THROW_EXCEPTION(header_parse_error()
                          << header_parse_error::message("Header without CRLF")
                          << header_parse_error::header_data(header_data));
  BOOST_ASSERT(header_data.size() >= 2);
  header_data.resize(header_data.size() - 2);
  parse(header_data);
  return size;
}

}  // namespace http
}  // namespace curl
}  // namespace bunsan
