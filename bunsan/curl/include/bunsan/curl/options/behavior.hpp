#pragma once

#include <bunsan/curl/options/wrapper/bool.hpp>
#include <bunsan/curl/options/wrapper/wrapped_option.hpp>

namespace bunsan {
namespace curl {
namespace options {

BUNSAN_CURL_OPTION_WRAPPED(verbose, CURLOPT_VERBOSE, bool_01)
BUNSAN_CURL_OPTION_WRAPPED(header, CURLOPT_HEADER, bool_01)
BUNSAN_CURL_OPTION_WRAPPED(noprogress, CURLOPT_NOPROGRESS, bool_01)
BUNSAN_CURL_OPTION_WRAPPED(nosignal, CURLOPT_NOSIGNAL, bool_01)
BUNSAN_CURL_OPTION_WRAPPED(wildcardmatch, CURLOPT_WILDCARDMATCH, bool_01)

}  // namespace options
}  // namespace curl
}  // namespace bunsan
