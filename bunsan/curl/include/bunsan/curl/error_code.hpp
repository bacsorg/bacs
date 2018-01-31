#pragma once

#include <curl/curl.h>
#include <curl/multi.h>

#include <system_error>

namespace std {

template <>
struct is_error_code_enum<CURLcode> : public true_type {};

template <>
struct is_error_code_enum<CURLMcode> : public true_type {};

template <>
struct is_error_code_enum<CURLSHcode> : public true_type {};

}  // namespace std
