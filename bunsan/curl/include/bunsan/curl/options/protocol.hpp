#pragma once

#include <bunsan/curl/options/wrapper/bool.hpp>
#include <bunsan/curl/options/wrapper/integer.hpp>
#include <bunsan/curl/options/wrapper/string.hpp>
#include <bunsan/curl/options/wrapper/wrapped_option.hpp>
#include <bunsan/curl/timecond.hpp>

namespace bunsan {
namespace curl {
namespace options {

BUNSAN_CURL_OPTION_WRAPPED(transfertext, CURLOPT_TRANSFERTEXT, bool_01)
BUNSAN_CURL_OPTION_WRAPPED(proxy_transfer_mode, CURLOPT_PROXY_TRANSFER_MODE,
                           bool_01)
BUNSAN_CURL_OPTION_WRAPPED(crlf, CURLOPT_CRLF, bool_01)
BUNSAN_CURL_OPTION_WRAPPED(range, CURLOPT_RANGE, string)  // X-Y,N-M
BUNSAN_CURL_OPTION_WRAPPED(resume_from, CURLOPT_RESUME_FROM_LARGE, curl_off_t_)
BUNSAN_CURL_OPTION_WRAPPED(customrequest, CURLOPT_CUSTOMREQUEST, string)
BUNSAN_CURL_OPTION_WRAPPED(filetime, CURLOPT_FILETIME, bool_01)
BUNSAN_CURL_OPTION_WRAPPED(nobody, CURLOPT_NOBODY, bool_01)
BUNSAN_CURL_OPTION_WRAPPED(infilesize, CURLOPT_INFILESIZE_LARGE, curl_off_t_)
BUNSAN_CURL_OPTION_WRAPPED(upload, CURLOPT_UPLOAD, bool_01)
BUNSAN_CURL_OPTION_WRAPPED(maxfilesize, CURLOPT_MAXFILESIZE_LARGE, curl_off_t_)
BUNSAN_CURL_OPTION_WRAPPED(timecondition, CURLOPT_TIMECONDITION,
                           enum_<timecond>)
BUNSAN_CURL_OPTION_WRAPPED(timevalue, CURLOPT_TIMEVALUE, time_t_)

}  // namespace options
}  // namespace curl
}  // namespace bunsan
