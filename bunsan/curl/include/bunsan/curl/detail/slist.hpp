#pragma once

#include <curl/curl.h>

#include <string>
#include <memory>

namespace bunsan::curl::detail {

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

}  // namespace bunsan::curl::detail
