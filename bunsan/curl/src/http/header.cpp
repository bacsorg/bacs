#include <bunsan/curl/http/header.hpp>

#include <bunsan/curl/http/error.hpp>

#include <boost/algorithm/string/case_conv.hpp>
#include <boost/assert.hpp>
#include <boost/io/detail/quoted_manip.hpp>
#include <boost/regex.hpp>

namespace bunsan {
namespace curl {
namespace http {

std::string header::merger::operator()(const std::string &value) const {
  BOOST_ASSERT(m_name);
  if (value.empty())
    return *m_name + ':';
  else
    return *m_name + ": " + value;
}

const std::string &header::value() const {
  if (!unique_value())
    BOOST_THROW_EXCEPTION(header_non_unique_value_error()
                          << header_non_unique_value_error::name(name()));
  return m_values[0];
}

header::values_const_range header::values() const {
  return values_const_range(m_values.begin(), m_values.end());
}

header::plain_const_range header::plain_headers() const {
  return plain_const_range(
      plain_const_iterator(m_values.begin(), merger(&m_name)),
      plain_const_iterator(m_values.end(), merger(&m_name)));
}

void header::merge(const header &h) {
  if (name() != h.name())
    BOOST_THROW_EXCEPTION(
        header_merge_incompatible_name_error()
        << header_merge_incompatible_name_error::name(name())
        << header_merge_incompatible_name_error::other_name(h.name()));
  m_values.insert(m_values.end(), h.m_values.begin(), h.m_values.end());
}

void header::merge(header &&h) {
  if (name() != h.name())
    BOOST_THROW_EXCEPTION(
        header_merge_incompatible_name_error()
        << header_merge_incompatible_name_error::name(name())
        << header_merge_incompatible_name_error::other_name(h.name()));
  for (std::string &value : h.m_values) m_values.push_back(std::move(value));
  h.m_name.clear();
  h.m_values.clear();
}

std::string header::make_name(const std::string &raw_name) {
  return boost::algorithm::to_lower_copy(raw_name);
}

header header::parse(const std::string &data) {
  static const boost::regex general_header(
      "^(?<name>[^ ]+)"
      " *: *"
      "(?<value>([^ ](.*[^ ])?)?)"
      " *$");
  boost::smatch result;
  if (boost::regex_match(data, result, general_header)) {
    const boost::ssub_match name = result["name"];
    const boost::ssub_match value = result["value"];
    BOOST_ASSERT(name.matched);
    BOOST_ASSERT(value.matched);
    return header{make_name(name), value};
  }
  BOOST_THROW_EXCEPTION(header_parse_error()
                        << header_parse_error::message("Not a header")
                        << header_parse_error::header_data(data));
}

std::ostream &operator<<(std::ostream &out, const header &h) {
  out << h.name() << ':';
  if (h.unique_value()) {
    if (!h.value().empty()) out << ' ';
    out << h.value();
  } else {
    out << " [";
    bool first = true;
    for (const std::string &value : h.values()) {
      if (!first) out << ", ";
      first = false;
      out << boost::io::quoted(value);
    }
    out << ']';
  }
  return out;
}

}  // namespace http
}  // namespace curl
}  // namespace bunsan
