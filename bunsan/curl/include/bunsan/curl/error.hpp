#pragma once

#include <bunsan/curl/error_category.hpp>
#include <bunsan/curl/error_code.hpp>

#include <bunsan/categorized_error.hpp>

#include <curl/curl.h>

namespace bunsan {
namespace curl {

struct error : virtual bunsan::error {};

template <typename Code, const std::error_category &(*GetCategory)()>
struct basic_error : categorized_error, virtual error {
  basic_error() = default;

  explicit basic_error(const Code code)
      : basic_error(std::error_code(code, GetCategory())) {}

  basic_error(const Code code, const std::string &what)
      : basic_error(std::error_code(code, GetCategory()), what) {}

  using categorized_error::categorized_error;
};

struct easy_error : basic_error<CURLcode, &easy_category> {
  using basic_error::basic_error;
};

struct multi_error : basic_error<CURLMcode, &multi_category> {
  using basic_error::basic_error;
};

struct share_error : basic_error<CURLSHcode, &share_category> {
  using basic_error::basic_error;
};

}  // namespace curl
}  // namespace bunsan

inline std::error_code make_error_code(const CURLcode e) {
  return std::error_code(e, bunsan::curl::easy_category());
}

inline std::error_code make_error_code(const CURLMcode e) {
  return std::error_code(e, bunsan::curl::multi_category());
}

inline std::error_code make_error_code(const CURLSHcode e) {
  return std::error_code(e, bunsan::curl::share_category());
}
