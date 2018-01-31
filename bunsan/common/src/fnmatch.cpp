#include <bunsan/fnmatch.hpp>

#include <cctype>

namespace bunsan {

fnmatcher::fnmatcher(const std::string &pattern, const flag flags)
    : m_regex(translate_to_regex(pattern), flags) {}

bool fnmatcher::operator()(const std::string &string) const {
  return boost::regex_match(string, m_regex);
}

namespace {
class parser {
 public:
  void operator()(const char c) {
    switch (state) {
      case text:
        switch (c) {
          case '*':
            result += ".*";
            break;
          case '?':
            result += ".";
            break;
          case '[':
            choice.clear();
            state = in_choice_first;
            break;
          default:
            if (!std::isalnum(c)) result.push_back('\\');
            result.push_back(c);
        }
        break;
      case in_choice_first:
        switch (c) {
          case '!':
            choice.push_back(c);
            state = in_choice_second;
            break;
          case ']':
            choice.push_back(c);
            state = in_choice_any;
            break;
          default:
            choice.push_back(c);
            state = in_choice_any;
        }
        break;
      case in_choice_second:
        switch (c) {
          case ']':
            choice.push_back(c);
            state = in_choice_any;
            break;
          default:
            choice.push_back(c);
            state = in_choice_any;
        }
        break;
      case in_choice_any:
        switch (c) {
          case ']':
            // commit choice
            result.push_back('[');
            {
              bool first = true;
              for (const char u : choice) {
                if (first) {
                  switch (u) {
                    case '!':
                      result.push_back('^');
                      break;
                    case '^':
                    case '\\':
                      result.push_back('\\');
                      result.push_back(u);
                      break;
                    default:
                      result.push_back(u);
                  }
                  first = false;
                } else {
                  if (u == '\\') result.push_back('\\');
                  result.push_back(u);
                }
              }
            }
            result.push_back(']');
            state = text;
            break;
          default:
            choice.push_back(c);
        }
        break;
      case end:
        // NOOP
        break;
    }
  }
  std::string finish() {
    switch (state) {
      case in_choice_first:
      case in_choice_second:
      case in_choice_any:
        result += "\\[";
        state = text;
        for (const char c : choice) (*this)(c);
        break;
      case text:
      case end:
        // NOOP
        break;
    }
    return result;
  }

 private:
  std::string result;
  std::string choice;
  enum state_type {
    text,
    in_choice_first,
    in_choice_second,
    in_choice_any,
    end
  } state = text;
};
}  // namespace

std::string fnmatcher::translate_to_regex(const std::string &pattern) {
  parser p;
  for (const char c : pattern) p(c);
  return p.finish();
}

}  // namespace bunsan
