#pragma once

#include <bunsan/curl/options/option_set.hpp>
#include <bunsan/curl/options/traits.hpp>

#include <curl/curl.h>

#include <string>
#include <system_error>
#include <type_traits>

namespace bunsan {
namespace curl {

class easy {
 public:
  /// Create invalid object.
  easy(std::nullptr_t) noexcept;

  /// Destroy current object making it invalid.
  easy &operator=(std::nullptr_t) noexcept;

  /// \note Takes ownership.
  explicit easy(CURL *const curl) noexcept;

  easy(const easy &);
  easy &operator=(const easy &);

  easy(easy &&) noexcept;
  easy &operator=(easy &&) noexcept;

  void swap(easy &) noexcept;

  explicit operator bool() const noexcept;

  /*!
   * \return raw handle
   *
   * \note Ownership remains.
   *
   * \warning If object is not valid behavior is undefined.
   */
  CURL *handle();

  easy();
  ~easy();

  std::string escape(const std::string &url);
  std::string unescape(const std::string &url);

  void perform();
  void perform(std::error_code &ec);

  void pause(const int bitmask);
  void pause(const int bitmask, std::error_code &ec);

  template <typename Option>
  void set(const Option &opt) {
    m_option_set.setopt(m_curl, opt);
  }

  /// Set all options to default values.
  void reset();

  /*!
   * Get easy object from captured CURL pointer.
   *
   * \warning If CURL pointer is not captured
   * by easy object behavior is undefined.
   */
  static easy &get(CURL *const curl) noexcept;

 private:
  void init() noexcept;

  static easy *get_(CURL *const curl) noexcept;

 private:
  /// \note implementation can use m_curl == nullptr internally
  CURL *m_curl;
  options::option_set m_option_set;
};

inline void swap(easy &a, easy &b) noexcept { a.swap(b); }

}  // namespace curl
}  // namespace bunsan
