#pragma once

#include <curl/curl.h>

namespace bunsan {
namespace curl {

enum class proxy : long {
  http = CURLPROXY_HTTP,
  http_1_0 = CURLPROXY_HTTP_1_0,
  socks4 = CURLPROXY_SOCKS4,
  socks5 = CURLPROXY_SOCKS5,
  socks4a = CURLPROXY_SOCKS4A,
  socks5_hostname = CURLPROXY_SOCKS5_HOSTNAME
};

}  // namespace curl
}  // namespace bunsan
