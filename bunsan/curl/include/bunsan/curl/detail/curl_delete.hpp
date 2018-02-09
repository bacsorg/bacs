#pragma once

namespace bunsan::curl::detail {

struct curl_delete {
  constexpr curl_delete() noexcept = default;

  void operator()(void *const ptr) const;
};

}  // namespace bunsan::curl::detail
