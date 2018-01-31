#pragma once

#include <bunsan/curl/content_encoding.hpp>
#include <bunsan/curl/http_version.hpp>
#include <bunsan/curl/options/wrapper/bitmask.hpp>
#include <bunsan/curl/options/wrapper/bool.hpp>
#include <bunsan/curl/options/wrapper/bytes.hpp>
#include <bunsan/curl/options/wrapper/csv_list.hpp>
#include <bunsan/curl/options/wrapper/enum_.hpp>
#include <bunsan/curl/options/wrapper/integer.hpp>
#include <bunsan/curl/options/wrapper/literal.hpp>
#include <bunsan/curl/options/wrapper/path.hpp>
#include <bunsan/curl/options/wrapper/stream_enum.hpp>
#include <bunsan/curl/options/wrapper/string.hpp>
#include <bunsan/curl/options/wrapper/string_list.hpp>
#include <bunsan/curl/options/wrapper/wrapped_bytes.hpp>
#include <bunsan/curl/options/wrapper/wrapped_option.hpp>
#include <bunsan/curl/redir_post.hpp>

namespace bunsan {
namespace curl {
namespace options {

BUNSAN_CURL_OPTION_WRAPPED(autoreferer, CURLOPT_AUTOREFERER, bool_01)
BUNSAN_CURL_OPTION_WRAPPED(accept_encoding, CURLOPT_ACCEPT_ENCODING,
                           stream_enum<content_encoding>)
BUNSAN_CURL_OPTION_WRAPPED(accept_encoding_all_available,
                           CURLOPT_ACCEPT_ENCODING, string_literal<>)
BUNSAN_CURL_OPTION_WRAPPED(transfer_encoding, CURLOPT_TRANSFER_ENCODING,
                           bool_01)
BUNSAN_CURL_OPTION_WRAPPED(followlocation, CURLOPT_FOLLOWLOCATION, bool_01)
BUNSAN_CURL_OPTION_WRAPPED(unrestricted_auth, CURLOPT_UNRESTRICTED_AUTH,
                           bool_01)
BUNSAN_CURL_OPTION_WRAPPED(maxredirs, CURLOPT_MAXREDIRS, long_)
BUNSAN_CURL_OPTION_WRAPPED(maxredirs_infinite, CURLOPT_MAXREDIRS,
                           long_literal<-1>)
BUNSAN_CURL_OPTION_WRAPPED(postredir, CURLOPT_POSTREDIR, bitmask<redir_post>)
BUNSAN_CURL_OPTION_WRAPPED(postredir_all, CURLOPT_POSTREDIR,
                           enum_literal<redir_post, redir_post::all>)
BUNSAN_CURL_OPTION_WRAPPED(put, CURLOPT_PUT, bool_01)
BUNSAN_CURL_OPTION_WRAPPED(post, CURLOPT_POST, bool_01)
BUNSAN_CURL_BYTES_WRAPPED(postfields, CURLOPT_POSTFIELDS,
                          CURLOPT_POSTFIELDSIZE_LARGE, bytes)
BUNSAN_CURL_OPTION_WRAPPED(postfieldsize, CURLOPT_POSTFIELDSIZE_LARGE,
                           curl_off_t_)
// retention_policy::by_curl version of postfields: CURLOPT_COPYPOSTFIELDS
// TODO BUNSAN_CURL_OPTION_WRAPPED(httppost, CURLOPT_HTTPPOST, httppost)
BUNSAN_CURL_OPTION_WRAPPED(referer, CURLOPT_REFERER, string)
BUNSAN_CURL_OPTION_WRAPPED(useragent, CURLOPT_USERAGENT, string)
BUNSAN_CURL_OPTION_WRAPPED(httpheader, CURLOPT_HTTPHEADER, string_list)
BUNSAN_CURL_OPTION_WRAPPED(http200aliases, CURLOPT_HTTP200ALIASES, string_list)
BUNSAN_CURL_OPTION_WRAPPED(cookie, CURLOPT_COOKIE, csv_list<';', ' '>)
BUNSAN_CURL_OPTION_WRAPPED(cookiefile, CURLOPT_COOKIEFILE, path)
BUNSAN_CURL_OPTION_WRAPPED(cookiejar, CURLOPT_COOKIEJAR, path)
BUNSAN_CURL_OPTION_WRAPPED(cookiejar_stdout, CURLOPT_COOKIEJAR,
                           string_literal<'-'>)
BUNSAN_CURL_OPTION_WRAPPED(cookielist, CURLOPT_COOKIELIST, string)
BUNSAN_CURL_OPTION_WRAPPED(httpget, CURLOPT_HTTPGET, bool_01)
BUNSAN_CURL_OPTION_WRAPPED(http_version, CURLOPT_HTTP_VERSION,
                           enum_<http_version>)
BUNSAN_CURL_OPTION_WRAPPED(ignore_content_length, CURLOPT_IGNORE_CONTENT_LENGTH,
                           bool_01)
BUNSAN_CURL_OPTION_WRAPPED(http_content_decoding, CURLOPT_HTTP_CONTENT_DECODING,
                           bool_01)
BUNSAN_CURL_OPTION_WRAPPED(http_transfer_decoding,
                           CURLOPT_HTTP_TRANSFER_DECODING, bool_01)

}  // namespace options
}  // namespace curl
}  // namespace bunsan
