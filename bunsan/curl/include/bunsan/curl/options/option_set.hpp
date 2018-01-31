#pragma once

#include <bunsan/curl/options/traits.hpp>

#include <curl/curl.h>

#include <boost/noncopyable.hpp>
#include <boost/range/any_range.hpp>

#include <array>
#include <memory>

namespace bunsan {
namespace curl {
namespace options {

namespace detail {
class option_base : private boost::noncopyable {
 public:
  using option_ptr = std::unique_ptr<option_base>;
  using shared_option_ptr = std::shared_ptr<option_base>;

  using const_id_range =
      boost::any_range<const CURLoption, boost::forward_traversal_tag,
                       const CURLoption &, std::ptrdiff_t>;

 public:
  virtual void setopt(CURL *curl) const = 0;
  virtual void unsetopt(CURL *curl) const noexcept = 0;

  virtual const_id_range ids() const = 0;

  virtual option_ptr clone() const = 0;

  virtual ~option_base();
};

using option_ptr = option_base::option_ptr;
using shared_option_ptr = option_base::shared_option_ptr;
}  // namespace detail

using detail::option_ptr;
using detail::shared_option_ptr;

template <typename T>
class option : public detail::option_base {
 public:
  template <typename Arg>
  explicit option(Arg &&arg)
      : m_impl(std::forward<Arg>(arg)) {}

  void setopt(CURL *curl) const override { m_impl.setopt(curl); }

  void unsetopt(CURL *curl) const noexcept override { m_impl.unsetopt(curl); }

  const_id_range ids() const override { return m_impl.ids(); }

  option_ptr clone() const override {
    option_ptr tmp(new option<T>(this->m_impl));
    return tmp;
  }

 private:
  T m_impl;
};

template <typename Option>
option_ptr make_option(Option &&opt) {
  option_ptr ptr(new option<typename option_traits<Option>::option_type>(
      std::forward<Option>(opt)));
  return ptr;
}

class option_set {
 public:
  option_set() = default;
  option_set(option_set &&) = default;
  option_set &operator=(option_set &&) = default;

  option_set(const option_set &);
  option_set &operator=(const option_set &);

  void swap(option_set &) noexcept;

  template <typename Option>
  void setopt(CURL *const curl, Option &&opt) {
    using retention_policy = typename option_traits<Option>::retention_policy;
    setopt_(curl, std::forward<Option>(opt), retention_policy());
  }

  void setopt(CURL *const curl) const;

  void clear();

 private:
  template <typename Option>
  void setopt_(CURL *const curl, Option &&opt, retention_policy::by_curl) {
    for (const CURLoption id : opt.ids()) unsetopt(curl, id);
    opt.setopt(curl);
  }

  template <typename Option>
  void setopt_(CURL *const curl, Option &&opt, retention_policy::by_wrapper) {
    store_and_setopt(make_option(std::forward<Option>(opt)), curl);
  }

  void store_and_setopt(option_ptr opt, CURL *const curl);
  void unsetopt(CURL *const curl, const CURLoption id);

  void reset(const CURLoption id);

 private:
  std::array<shared_option_ptr, CURLOPT_LASTENTRY> m_options;
};

inline void swap(option_set &a, option_set &b) noexcept { a.swap(b); }

}  // namespace options
}  // namespace curl
}  // namespace bunsan
