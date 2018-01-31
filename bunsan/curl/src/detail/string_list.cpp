#include <bunsan/curl/detail/string_list.hpp>

#include <cstring>

namespace bunsan {
namespace curl {
namespace detail {

string_list::string_list(const string_list &list) {
  for (const char *const obj : list) append(obj);
}

string_list &string_list::operator=(const string_list &list) {
  string_list(list).swap(*this);
  return *this;
}

bool operator==(const string_list &a, const string_list &b) {
  auto i = a.begin(), j = b.begin();
  for (; i != a.end() && j != b.end(); ++i, ++j) {
    if (std::strcmp(*i, *j)) return false;
  }
  return i == a.end() && j == b.end();
}

}  // namespace detail
}  // namespace curl
}  // namespace bunsan
