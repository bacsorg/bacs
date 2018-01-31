#pragma once

#include <bunsan/curl/options/wrapper/string.hpp>

#include <curl/curl.h>

#include <initializer_list>
#include <sstream>

namespace bunsan {
namespace curl {
namespace options {
namespace wrapper {

namespace detail {
template <char Separator, char... Separators>
struct csv_list_separators {
  static void call(std::ostream &out) {
    csv_list_separators<Separator>::call(out);
    csv_list_separators<Separators...>::call(out);
  }
};

template <char Separator>
struct csv_list_separators<Separator> {
  static void call(std::ostream &out) { out << Separator; }
};
}  // namespace detail

template <char Separator = ',', char... Separators>
class csv_list : public string {
 public:
  csv_list() = default;

  template <typename Container>
  explicit csv_list(const Container &container)
      : string(to_string(container)) {}

  template <typename T>
  explicit csv_list(const std::initializer_list<T> &container)
      : string(to_string(container)) {}

 private:
  /// \todo verify container items
  template <typename Container>
  static std::string to_string(const Container &container) {
    std::ostringstream sout;
    bool first = true;
    for (const auto &obj : container) {
      if (first) {
        first = false;
      } else {
        detail::csv_list_separators<Separator, Separators...>::call(sout);
      }
      sout << obj;
    }
    return sout.str();
  }
};

}  // namespace wrapper
}  // namespace options
}  // namespace curl
}  // namespace bunsan
