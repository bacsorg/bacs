#pragma once

#include <bunsan/curl/info/wrapper/getinfo.hpp>
#include <bunsan/curl/info/wrapper/primitive.hpp>

namespace bunsan {
namespace curl {
namespace info {

BUNSAN_CURL_GETINFO(total_time, CURLINFO_TOTAL_TIME, double_)
BUNSAN_CURL_GETINFO(namelookup_time, CURLINFO_NAMELOOKUP_TIME, double_)
BUNSAN_CURL_GETINFO(connect_time, CURLINFO_CONNECT_TIME, double_)
BUNSAN_CURL_GETINFO(appconnect_time, CURLINFO_APPCONNECT_TIME, double_)
BUNSAN_CURL_GETINFO(pretransfer_time, CURLINFO_PRETRANSFER_TIME, double_)
BUNSAN_CURL_GETINFO(starttransfer_time, CURLINFO_STARTTRANSFER_TIME, double_)
BUNSAN_CURL_GETINFO(redirect_time, CURLINFO_REDIRECT_TIME, double_)

}  // namespace info
}  // namespace curl
}  // namespace bunsan
