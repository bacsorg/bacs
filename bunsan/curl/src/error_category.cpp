#include <bunsan/curl/error_category.hpp>

#include <curl/curl.h>

namespace bunsan {
namespace curl {

namespace {
class easy_category_ : public std::error_category {
 public:
  const char *name() const noexcept override { return "curl_easy"; }

  std::string message(int code) const override {
    return ::curl_easy_strerror(static_cast<CURLcode>(code));
  }
};

class multi_category_ : public std::error_category {
 public:
  const char *name() const noexcept override { return "curl_multi"; }

  std::string message(int code) const override {
    return ::curl_multi_strerror(static_cast<CURLMcode>(code));
  }
};

class share_category_ : public std::error_category {
 public:
  const char *name() const noexcept override { return "curl_share"; }

  std::string message(int code) const override {
    return ::curl_share_strerror(static_cast<CURLSHcode>(code));
  }
};
}  // namespace

const std::error_category &easy_category() noexcept {
  static const easy_category_ impl{};
  return impl;
}

const std::error_category &multi_category() noexcept {
  static const multi_category_ impl{};
  return impl;
}

const std::error_category &share_category() noexcept {
  static const share_category_ impl{};
  return impl;
}

}  // namespace curl
}  // namespace bunsan
