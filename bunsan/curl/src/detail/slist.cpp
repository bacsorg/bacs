#include <bunsan/curl/detail/slist.hpp>

#include <curl/curl.h>

namespace bunsan {
namespace curl {
namespace detail {

void slist::deleter::operator()(void *const ptr) const {
  curl_slist_free_all(static_cast<struct ::curl_slist *>(ptr));
}

void slist::append(ptr &list, const char *const data) {
  using std::swap;

  ptr upd(::curl_slist_append(list.get(), data));
  if (upd) {
    swap(list, upd);
    upd.release();
  } else {
    throw std::bad_alloc();
  }
}

}  // namespace detail
}  // namespace curl
}  // namespace bunsan
