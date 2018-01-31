#include <bunsan/curl/http/status.hpp>

#include <bunsan/curl/http/error.hpp>

#include <boost/assert.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/regex.hpp>

namespace bunsan {
namespace curl {
namespace http {

status status::parse(const std::string &header) {
  static const boost::regex status_header(
      "^HTTP/(?<major>\\d+)\\.(?<minor>\\d+)"
      "("
          " "
          "("
              "(?<code>\\d+)"
              "("
                  " "
                  "(?<reason>.*)"
              ")?"
          ")?"
      ")?$");
  try {
    boost::smatch result;
    if (boost::regex_match(header, result, status_header)) {
      status ret;
      const boost::ssub_match major = result["major"];
      const boost::ssub_match minor = result["minor"];
      const boost::ssub_match code = result["code"];
      const boost::ssub_match reason = result["reason"];
      BOOST_ASSERT(major.matched);
      BOOST_ASSERT(minor.matched);
      ret.version = http_version_pair(boost::lexical_cast<unsigned>(major),
                                      boost::lexical_cast<unsigned>(minor));
      if (code.matched) {
        ret.code = boost::lexical_cast<unsigned>(code);
        if (reason.matched) ret.reason = reason;
      } else {
        BOOST_ASSERT(!reason.matched);
      }
      return ret;
    }
    BOOST_THROW_EXCEPTION(status_parse_error()
                          << status_parse_error::message("Not a status")
                          << status_parse_error::header_data(header));
  } catch (status_parse_error &) {
    throw;
  } catch (std::exception &) {
    BOOST_THROW_EXCEPTION(status_parse_error()
                          << status_parse_error::header_data(header)
                          << enable_nested_current());
  }
}

std::ostream &operator<<(std::ostream &out, const status &s) {
  out << "HTTP/" << http_version_pair(s.version);
  if (s.code) {
    out << " " << s.code;
    if (!s.reason.empty()) out << " " << s.reason;
  }
  return out;
}

}  // namespace http
}  // namespace curl
}  // namespace bunsan
