#include <bunsan/curl/http/header_set.hpp>

namespace bunsan {
namespace curl {
namespace http {

void header_set::merge_insert(const header &h) {
  const auto iter = index().find(h.name());
  if (iter != index().end()) {
    index().modify(iter, [&h](header &hdr) { hdr.merge(h); });
  } else {
    index().insert(h);
  }
}

void header_set::erase(const std::string &name) {
  index().erase(header::make_name(name));
}

header_set::const_iterator header_set::find(const std::string &name) const {
  return index().find(header::make_name(name));
}

header_set::plain_const_range header_set::plain_headers() const {
  return plain_const_range(plain_const_iterator(index().begin(), index().end()),
                           plain_const_iterator(index().end(), index().end()));
}

std::ostream &operator<<(std::ostream &out, const header_set &h) {
  for (const header &hdr : h)
    for (const std::string &str : hdr.plain_headers()) out << str << '\n';
  return out;
}

}  // namespace http
}  // namespace curl
}  // namespace bunsan
