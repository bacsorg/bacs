#pragma once

#include <bunsan/curl/info/wrapper/getinfo.hpp>
#include <bunsan/curl/info/wrapper/primitive.hpp>
#include <bunsan/curl/info/wrapper/string_range.hpp>

namespace bunsan {
namespace curl {
namespace info {

BUNSAN_CURL_GETINFO(ssl_verifyresult, CURLINFO_SSL_VERIFYRESULT, long_)
BUNSAN_CURL_GETINFO(ssl_engines, CURLINFO_SSL_ENGINES, string_range)
// BUNSAN_CURL_GETINFO(certinfo, CURLINFO_CERTINFO, curl_certinfo)
// BUNSAN_CURL_GETINFO(tls_session, CURLINFO_TLS_SESSION, curl_tlssessioninfo)

}  // namespace info
}  // namespace curl
}  // namespace bunsan
