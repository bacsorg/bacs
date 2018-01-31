#include <bunsan/curl/http/client.hpp>

#include <bunsan/curl/http/error.hpp>
#include <bunsan/curl/options/callback.hpp>
#include <bunsan/curl/options/http.hpp>
#include <bunsan/curl/options/network.hpp>
#include <bunsan/curl/options/protocol.hpp>

#include <boost/lexical_cast.hpp>

namespace bunsan {
namespace curl {
namespace http {

client::client() {
  set(m_header_parser.option());

  // suppress all output
  set_writefunction([](const char *, std::size_t size) { return size; });
}

void client::set_method_get() { set(options::httpget(true)); }

void client::set_method_head() { set(options::nobody(true)); }

void client::set_method_post() { set(options::post(true)); }

void client::set_method_put() { set(options::upload(true)); }

void client::set_useragent(const std::string &useragent) {
  set(options::useragent(useragent));
}

void client::set_referer(const std::string &referer) {
  set(options::referer(referer));
}

void client::set_header(const header &h) { m_request_headers.merge_insert(h); }

void client::set_header(const std::string &name, const std::string &value) {
  set_header(header(name, value));
}

void client::set_header(const std::string &h) { set_header(header::parse(h)); }

void client::unset_header(const std::string &name) {
  m_request_headers.erase(name);
}

void client::set_content_length(const std::size_t size) {
  set(options::postfieldsize(size));
}

void client::set_content_type(const std::string &content_type) {
  set_header("Content-Type", content_type);
}

void client::set_transfer_encoding(const std::string &transfer_encoding) {
  set_header("Transfer-Encoding", transfer_encoding);
}

void client::set_transfer_encoding_chunked() {
  set_transfer_encoding("chunked");
}

void client::set_url(const std::string &url) { set(options::url(url)); }

void client::set_follow_location(const bool follow_location) {
  set(options::followlocation(follow_location));
}

void client::set_readfunction(const readfunction &callback) {
  set_method_post();
  set(options::readfunction(callback));
}

void client::set_writefunction(const writefunction &callback) {
  set(options::writefunction(callback));
}

void client::set_post_data(const std::string &data) {
  set(options::postfields(data));
}

const header_parser &client::responses() { return m_header_parser; }

status client::perform() {
  m_easy.set(options::httpheader(m_request_headers.plain_headers()));
  m_easy.perform();
  return m_header_parser.last().status();
}

namespace {
const not_ok_status_error &fill_status_error(const not_ok_status_error &e,
                                             const status &s) {
  if (s.version != http_version::http_none)
    e << not_ok_status_error::status_http_version(s.version);
  if (s.code) e << not_ok_status_error::status_code(s.code);
  if (!s.reason.empty()) e << not_ok_status_error::status_reason(s.reason);
  return e;
}
}  // namespace

void client::check_perform() {
  const status s = perform();
  if (s.code != 200)
    BOOST_THROW_EXCEPTION(fill_status_error(not_ok_status_error(), s));
}

}  // namespace http
}  // namespace curl
}  // namespace bunsan
