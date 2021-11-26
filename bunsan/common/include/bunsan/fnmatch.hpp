#pragma once

#include <string>

#include <boost/regex.hpp>

namespace bunsan {

class fnmatcher {
 public:
  using flag = boost::regex_constants::syntax_option_type;
  static constexpr flag defaults = boost::regex_constants::ECMAScript;
  static constexpr flag icase = boost::regex_constants::icase;

  explicit fnmatcher(const std::string &pattern, flag flags = defaults);

  bool operator()(const std::string &string) const;

  static std::string translate_to_regex(const std::string &pattern);

 private:
  boost::regex m_regex;
};

inline bool fnmatch(const std::string &pattern, const std::string &string,
                    fnmatcher::flag flags = fnmatcher::defaults) {
  return fnmatcher(pattern, flags)(string);
}

}  // namespace bunsan
