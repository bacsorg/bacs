#pragma once

#include <bunsan/curl/options/wrapper/bitmask.hpp>
#include <bunsan/curl/options/wrapper/bool.hpp>
#include <bunsan/curl/options/wrapper/csv_list.hpp>
#include <bunsan/curl/options/wrapper/duration.hpp>
#include <bunsan/curl/options/wrapper/enum_.hpp>
#include <bunsan/curl/options/wrapper/integer.hpp>
#include <bunsan/curl/options/wrapper/literal.hpp>
#include <bunsan/curl/options/wrapper/string.hpp>
#include <bunsan/curl/options/wrapper/wrapped_option.hpp>
#include <bunsan/curl/options/wrapper/wrapped_option_default.hpp>

#include <bunsan/curl/proto.hpp>
#include <bunsan/curl/proxy.hpp>

namespace bunsan {
namespace curl {
namespace options {

BUNSAN_CURL_OPTION_WRAPPED(url, CURLOPT_URL, string)
BUNSAN_CURL_OPTION_WRAPPED(protocols, CURLOPT_PROTOCOLS, bitmask<proto>)
BUNSAN_CURL_OPTION_WRAPPED(protocols_all, CURLOPT_PROTOCOLS,
                           enum_literal<proto, proto::all>)
BUNSAN_CURL_OPTION_WRAPPED(redir_protocols, CURLOPT_REDIR_PROTOCOLS,
                           bitmask<proto>)
BUNSAN_CURL_OPTION_WRAPPED(redir_protocols_all, CURLOPT_REDIR_PROTOCOLS,
                           enum_literal<proto, proto::all>)
BUNSAN_CURL_OPTION_WRAPPED(proxy, CURLOPT_PROXY, string)
BUNSAN_CURL_OPTION_WRAPPED(proxyport, CURLOPT_PROXYPORT, long_)
BUNSAN_CURL_OPTION_WRAPPED(proxytype, CURLOPT_PROXYTYPE, enum_<curl::proxy>)
BUNSAN_CURL_OPTION_WRAPPED(noproxy, CURLOPT_NOPROXY, csv_list<>)
BUNSAN_CURL_OPTION_WRAPPED(httpproxytunnel, CURLOPT_HTTPPROXYTUNNEL, bool_01)
BUNSAN_CURL_OPTION_WRAPPED(socks5_gssapi_service, CURLOPT_SOCKS5_GSSAPI_SERVICE,
                           string)
BUNSAN_CURL_OPTION_WRAPPED(socks5_gssapi_nec, CURLOPT_SOCKS5_GSSAPI_NEC,
                           bool_01)
BUNSAN_CURL_OPTION_WRAPPED(interface, CURLOPT_INTERFACE, string)
BUNSAN_CURL_OPTION_WRAPPED(localport, CURLOPT_LOCALPORT, long_)
BUNSAN_CURL_OPTION_WRAPPED(localportrange, CURLOPT_LOCALPORTRANGE, long_)
BUNSAN_CURL_OPTION_DEFAULT_WRAPPED(dns_cache_timeout, CURLOPT_DNS_CACHE_TIMEOUT,
                                   seconds, -1)
// obsolete BUNSAN_CURL_OPTION_WRAPPED(dns_use_global_cache,
// CURLOPT_DNS_USE_GLOBAL_CACHE, bool_01)
BUNSAN_CURL_OPTION_WRAPPED(buffersize, CURLOPT_BUFFERSIZE,
                           long_)  // TODO Do we need negative checks?
BUNSAN_CURL_OPTION_WRAPPED(port, CURLOPT_PORT, long_)
BUNSAN_CURL_OPTION_WRAPPED(tcp_nodelay, CURLOPT_TCP_NODELAY, bool_01)
BUNSAN_CURL_OPTION_WRAPPED(address_scope, CURLOPT_ADDRESS_SCOPE, long_)
BUNSAN_CURL_OPTION_WRAPPED(tcp_keepalive, CURLOPT_TCP_KEEPALIVE, bool_01)
BUNSAN_CURL_OPTION_WRAPPED(tcp_keepidle, CURLOPT_TCP_KEEPIDLE, seconds)
BUNSAN_CURL_OPTION_WRAPPED(tcp_keepintvl, CURLOPT_TCP_KEEPINTVL, seconds)

}  // namespace options
}  // namespace curl
}  // namespace bunsan
