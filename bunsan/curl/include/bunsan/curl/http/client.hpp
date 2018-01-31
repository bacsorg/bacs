#pragma once

#include <bunsan/curl/easy.hpp>
#include <bunsan/curl/http/header_parser.hpp>
#include <bunsan/curl/http/header_set.hpp>
#include <bunsan/curl/http/status.hpp>
#include <bunsan/curl/options/wrapper/readfunction.hpp>
#include <bunsan/curl/options/wrapper/writefunction.hpp>

#include <boost/noncopyable.hpp>

#include <utility>

namespace bunsan {
namespace curl {
namespace http {

class client : private boost::noncopyable {
 public:
  using readfunction = options::wrapper::readfunction::function_type;
  using writefunction = options::wrapper::writefunction::function_type;

 public:
  client();

  template <typename Option>
  void set(Option &&option) {
    m_easy.set(std::forward<Option>(option));
  }

  void set_method_get();
  void set_method_head();
  void set_method_post();
  void set_method_put();

  // TODO cookies

  void set_useragent(const std::string &useragent);
  void set_referer(const std::string &referer);

  void set_header(const header &h);
  void set_header(const std::string &name, const std::string &value);
  void set_header(const std::string &header);
  void unset_header(const std::string &name);
  void set_content_length(const std::size_t size);
  void set_content_type(const std::string &content_type);
  void set_transfer_encoding(const std::string &transfer_encoding);
  void set_transfer_encoding_chunked();

  void set_url(const std::string &url);

  void set_follow_location(const bool follow_location);

  void set_readfunction(const readfunction &callback);
  void set_writefunction(const writefunction &callback);

  void set_post_data(const std::string &data);

  const header_parser &responses();

  status perform();

  /// \throws not_ok_status_error if status.code != 200
  void check_perform();

 private:
  easy m_easy;
  header_set m_request_headers;
  header_parser m_header_parser;
};

}  // namespace http
}  // namespace curl
}  // namespace bunsan
