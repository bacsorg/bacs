#pragma once

#include <bunsan/curl/ipresolve.hpp>
#include <bunsan/curl/options/wrapper/bool.hpp>
#include <bunsan/curl/options/wrapper/duration.hpp>
#include <bunsan/curl/options/wrapper/enum_.hpp>
#include <bunsan/curl/options/wrapper/integer.hpp>
#include <bunsan/curl/options/wrapper/string.hpp>
#include <bunsan/curl/options/wrapper/string_list.hpp>
#include <bunsan/curl/options/wrapper/wrapped_option.hpp>
#include <bunsan/curl/usessl.hpp>

namespace bunsan {
namespace curl {
namespace options {

BUNSAN_CURL_OPTION_WRAPPED(timeout, CURLOPT_TIMEOUT_MS, milliseconds)
BUNSAN_CURL_OPTION_WRAPPED(low_speed_limit, CURLOPT_LOW_SPEED_LIMIT, long_)
BUNSAN_CURL_OPTION_WRAPPED(low_speed_time, CURLOPT_LOW_SPEED_TIME, seconds)
BUNSAN_CURL_OPTION_WRAPPED(max_send_speed_large, CURLOPT_MAX_SEND_SPEED_LARGE,
                           curl_off_t_)
BUNSAN_CURL_OPTION_WRAPPED(max_recv_speed_large, CURLOPT_MAX_RECV_SPEED_LARGE,
                           curl_off_t_)
BUNSAN_CURL_OPTION_WRAPPED(maxconnects, CURLOPT_MAXCONNECTS, long_)
BUNSAN_CURL_OPTION_WRAPPED(forbid_reuse, CURLOPT_FORBID_REUSE, bool_01)
BUNSAN_CURL_OPTION_WRAPPED(connecttimeout, CURLOPT_CONNECTTIMEOUT, seconds)
BUNSAN_CURL_OPTION_WRAPPED(connecttimeout_ms, CURLOPT_CONNECTTIMEOUT_MS,
                           milliseconds)
BUNSAN_CURL_OPTION_WRAPPED(ipresolve, CURLOPT_IPRESOLVE, enum_<ipresolve>)
BUNSAN_CURL_OPTION_WRAPPED(connect_only, CURLOPT_CONNECT_ONLY, bool_01)
BUNSAN_CURL_OPTION_WRAPPED(use_ssl, CURLOPT_USE_SSL, enum_<usessl>)
BUNSAN_CURL_OPTION_WRAPPED(resolve, CURLOPT_RESOLVE,
                           string_list)  // HOST:PORT:ADDRESS, -HOST:PORT
BUNSAN_CURL_OPTION_WRAPPED(dns_servers, CURLOPT_DNS_SERVERS,
                           string)  // host[:port][,host[:port]]
BUNSAN_CURL_OPTION_WRAPPED(accepttimeout, CURLOPT_ACCEPTTIMEOUT_MS,
                           milliseconds)

}  // namespace options
}  // namespace curl
}  // namespace bunsan
