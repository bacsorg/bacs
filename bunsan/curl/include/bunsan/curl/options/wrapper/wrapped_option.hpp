#pragma once

#include <bunsan/curl/detail/easy.hpp>
#include <bunsan/curl/detail/one_step_iterator.hpp>
#include <bunsan/curl/error.hpp>

#include <curl/curl.h>

#include <boost/range/iterator_range.hpp>

#include <type_traits>

namespace bunsan {
namespace curl {
namespace options {
namespace wrapper {

namespace detail {
template <typename RetentionPolicy>
struct unsetter;

template <>
struct unsetter<retention_policy::by_curl> {
  template <typename Data>
  static void unsetopt(CURL *, const CURLoption, const Data &) noexcept {}
};

template <>
struct unsetter<retention_policy::by_wrapper> {
  template <typename Data>
  static void unsetopt(CURL *const curl, const CURLoption id,
                       const Data &) noexcept {
    static_assert(std::is_pointer<Data>::value, "Only pointers are supported!");
    ::curl_easy_setopt(curl, id, static_cast<Data>(nullptr));
  }
};
}  // namespace detail

template <CURLoption Id, typename Wrapper>
class wrapped_option : private Wrapper {
 public:
  using retention_policy = typename Wrapper::retention_policy;

  using Wrapper::Wrapper;

  constexpr CURLoption id() const { return Id; }

  curl::detail::one_step_range<const CURLoption> ids() const {
    return curl::detail::make_one_step_range<const CURLoption>(id());
  }

  void setopt(CURL *const curl) const {
    curl::detail::easy::setopt(curl, id(), Wrapper::data());
  }

  void unsetopt(CURL *const curl) const {
    detail::unsetter<retention_policy>::unsetopt(curl, id(), Wrapper::data());
  }
};

}  // namespace wrapper
}  // namespace options
}  // namespace curl
}  // namespace bunsan

#define BUNSAN_CURL_OPTION_WRAPPED(NAME, CODE, ...)              \
  using NAME = ::bunsan::curl::options::wrapper::wrapped_option< \
      CODE, ::bunsan::curl::options::wrapper::__VA_ARGS__>;
