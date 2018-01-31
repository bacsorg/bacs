#pragma once

#include <bunsan/curl/http/response_head.hpp>
#include <bunsan/curl/options/callback.hpp>

#include <boost/optional.hpp>
#include <boost/range/iterator_range.hpp>

#include <functional>
#include <vector>

namespace bunsan {
namespace curl {
namespace http {

class header_parser {
 private:
  using map_type = std::vector<response_head>;

 public:
  using value_type = map_type::value_type;
  using const_iterator = map_type::const_iterator;
  using const_range = boost::iterator_range<const_iterator>;
  using option_type = options::headerfunction;
  using callback_type = option_type::function_type;

 public:
  header_parser() = default;

  callback_type callback();
  option_type option();

  const_range responses() const { return const_range(begin(), end()); }
  const_iterator begin() const { return m_responses.begin(); }
  const_iterator end() const { return m_responses.end(); }

  bool empty() const { return m_responses.empty(); }
  std::size_t size() const { return m_responses.size(); }

  const response_head &at(const std::size_t index) const;
  const response_head &operator[](const std::size_t index) const {
    return at(index);
  }

  const response_head &front() const;
  const response_head &first() const { return front(); }

  const response_head &back() const;
  const response_head &last() const { return back(); }

  /// Status line or header without CRLF
  void parse(const std::string &header_data);

 private:
  std::size_t parse_header_callback(const char *ptr, std::size_t size);

 private:
  std::vector<response_head> m_responses;
};

inline header_parser::const_iterator begin(const header_parser &p) {
  return p.begin();
}

inline header_parser::const_iterator end(const header_parser &p) {
  return p.end();
}

}  // namespace http
}  // namespace curl
}  // namespace bunsan
