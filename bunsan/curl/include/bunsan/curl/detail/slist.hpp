#pragma once

#include <curl/curl.h>

#include <memory>

namespace bunsan {
namespace curl {
namespace detail {

namespace slist {
struct deleter {
  constexpr deleter() noexcept = default;

  void operator()(void *const ptr) const;
};

using ptr = std::unique_ptr<struct ::curl_slist, deleter>;

void append(ptr &list, const char *const data);

inline void append(ptr &list, const std::string &data) {
  append(list, data.c_str());
}
}  // namespace slist

using slist_ptr = slist::ptr;

}  // namespace detail
}  // namespace curl
}  // namespace bunsan
