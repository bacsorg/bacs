#pragma once

#include <bunsan/curl/info/wrapper/bool_.hpp>
#include <bunsan/curl/info/wrapper/getinfo.hpp>
#include <bunsan/curl/info/wrapper/optional_string.hpp>
#include <bunsan/curl/info/wrapper/primitive.hpp>
#include <bunsan/curl/info/wrapper/string.hpp>
#include <bunsan/curl/info/wrapper/string_range.hpp>

namespace bunsan {
namespace curl {
namespace info {

BUNSAN_CURL_GETINFO(effective_url, CURLINFO_EFFECTIVE_URL, string)
BUNSAN_CURL_GETINFO(response_code, CURLINFO_RESPONSE_CODE, long_)
BUNSAN_CURL_GETINFO(http_connectcode, CURLINFO_HTTP_CONNECTCODE, long_)
BUNSAN_CURL_GETINFO(filetime, CURLINFO_FILETIME, long_)

BUNSAN_CURL_GETINFO(redirect_count, CURLINFO_REDIRECT_COUNT, long_)
BUNSAN_CURL_GETINFO(redirect_url, CURLINFO_REDIRECT_URL, string)

BUNSAN_CURL_GETINFO(size_upload, CURLINFO_SIZE_UPLOAD, double_)
BUNSAN_CURL_GETINFO(size_download, CURLINFO_SIZE_DOWNLOAD, double_)

BUNSAN_CURL_GETINFO(speed_download, CURLINFO_SPEED_DOWNLOAD, double_)
BUNSAN_CURL_GETINFO(speed_upload, CURLINFO_SPEED_UPLOAD, double_)

BUNSAN_CURL_GETINFO(header_size, CURLINFO_HEADER_SIZE, long_)
BUNSAN_CURL_GETINFO(request_size, CURLINFO_REQUEST_SIZE, long_)

BUNSAN_CURL_GETINFO(content_length_download, CURLINFO_CONTENT_LENGTH_DOWNLOAD,
                    double_)
BUNSAN_CURL_GETINFO(content_length_upload, CURLINFO_CONTENT_LENGTH_UPLOAD,
                    double_)
BUNSAN_CURL_GETINFO(content_type, CURLINFO_CONTENT_TYPE, optional_string)

// BUNSAN_CURL_GETINFO(httpauth_avail, CURLINFO_HTTPAUTH_AVAIL, curlauth)
// BUNSAN_CURL_GETINFO(proxyauth_avail, CURLINFO_PROXYAUTH_AVAIL, proxyauth)

BUNSAN_CURL_GETINFO(os_errno, CURLINFO_OS_ERRNO, long_)

BUNSAN_CURL_GETINFO(num_connects, CURLINFO_NUM_CONNECTS, long_)
BUNSAN_CURL_GETINFO(primary_ip, CURLINFO_PRIMARY_IP, string)
BUNSAN_CURL_GETINFO(primary_port, CURLINFO_PRIMARY_PORT, long_)
BUNSAN_CURL_GETINFO(local_ip, CURLINFO_LOCAL_IP, string)
BUNSAN_CURL_GETINFO(local_port, CURLINFO_LOCAL_PORT, long_)

BUNSAN_CURL_GETINFO(cookielist, CURLINFO_COOKIELIST, string_range)

// note: see libcurl documentation about win64
BUNSAN_CURL_GETINFO(lastsocket, CURLINFO_LASTSOCKET, long_)

BUNSAN_CURL_GETINFO(ftp_entry_path, CURLINFO_FTP_ENTRY_PATH, optional_string)

BUNSAN_CURL_GETINFO(condition_unmet, CURLINFO_CONDITION_UNMET, bool_)

}  // namespace info
}  // namespace curl
}  // namespace bunsan
