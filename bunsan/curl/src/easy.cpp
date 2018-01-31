#include <bunsan/curl/easy.hpp>

#include <bunsan/curl/error.hpp>

#include <curl/curl.h>

#include <boost/assert.hpp>

namespace bunsan {
namespace curl {

easy::easy(std::nullptr_t) noexcept : m_curl(nullptr) {}

easy &easy::operator=(std::nullptr_t) noexcept {
  if (m_curl) ::curl_easy_cleanup(m_curl);
  m_curl = nullptr;
  m_option_set.clear();
  return *this;
}

easy::easy(CURL *const curl) noexcept : m_curl(curl) {
  BOOST_ASSERT_MSG(!get_(curl), "CURL is already captured.");
  init();
}

easy::easy(const easy &curl)
    : m_curl(curl.m_curl ? ::curl_easy_duphandle(curl.m_curl) : nullptr),
      m_option_set(curl.m_option_set) {
  if (curl.m_curl) {
    if (!m_curl)
      BOOST_THROW_EXCEPTION(easy_error()
                            << easy_error::what_message("curl_easy_duphandle"));
    init();
    m_option_set.setopt(m_curl);
  }
}

easy &easy::operator=(const easy &curl) {
  easy(curl).swap(*this);
  return *this;
}

easy::easy(easy &&curl) noexcept : m_curl(curl.m_curl),
                                   m_option_set(std::move(curl.m_option_set)) {
  curl.m_curl = nullptr;
  init();
}

easy &easy::operator=(easy &&curl) noexcept {
  m_curl = curl.m_curl;
  curl.m_curl = nullptr;
  m_option_set = std::move(curl.m_option_set);
  init();
  return *this;
}

easy::easy() : m_curl(::curl_easy_init()) {
  if (!m_curl)
    BOOST_THROW_EXCEPTION(easy_error()
                          << easy_error::what_message("curl_easy_init"));
  init();
}

easy::~easy() { *this = nullptr; }

void easy::swap(easy &curl) noexcept {
  using std::swap;

  swap(m_curl, curl.m_curl);
  swap(m_option_set, curl.m_option_set);
  init();
  curl.init();
}

easy::operator bool() const noexcept { return m_curl; }

CURL *easy::handle() {
  BOOST_ASSERT(*this);

  return m_curl;
}

void easy::perform() {
  BOOST_ASSERT(*this);

  std::error_code ec;
  perform(ec);
  if (ec) BOOST_THROW_EXCEPTION(easy_error(ec, "curl_easy_perform"));
}

void easy::perform(std::error_code &ec) {
  BOOST_ASSERT(*this);

  ec.assign(::curl_easy_perform(m_curl), easy_category());
}

void easy::pause(const int bitmask) {
  BOOST_ASSERT(*this);

  std::error_code ec;
  pause(bitmask, ec);
  if (ec) BOOST_THROW_EXCEPTION(easy_error(ec, "curl_easy_pause"));
}

void easy::pause(const int bitmask, std::error_code &ec) {
  BOOST_ASSERT(*this);

  ec.assign(::curl_easy_pause(m_curl, bitmask), easy_category());
}

void easy::reset() {
  ::curl_easy_reset(m_curl);
  m_option_set.clear();
}

void easy::init() noexcept {
  if (*this) {
    const CURLcode ret = ::curl_easy_setopt(m_curl, CURLOPT_PRIVATE, this);
    BOOST_ASSERT_MSG(ret == CURLE_OK, easy_category().message(ret).c_str());
  }
}

easy &easy::get(CURL *const curl) noexcept {
  easy *const obj = get_(curl);
  BOOST_ASSERT_MSG(obj, "CURL was not captured.");
  BOOST_ASSERT(obj->m_curl == curl);
  return *obj;
}

easy *easy::get_(CURL *const curl) noexcept {
  BOOST_ASSERT(curl);

  char *obj;
  const CURLcode ret = ::curl_easy_getinfo(curl, CURLINFO_PRIVATE, &obj);
  BOOST_ASSERT_MSG(ret == CURLE_OK, easy_category().message(ret).c_str());
  return reinterpret_cast<easy *>(obj);
}

}  // namespace curl
}  // namespace bunsan
