#pragma once

#include <bunsan/curl/detail/curl_delete.hpp>

#include <memory>

namespace bunsan::curl::detail {

template <typename T>
using unique_ptr = std::unique_ptr<T, curl_delete>;

}  // namespace bunsan::curl::detail
