#pragma once

namespace bunsan {
namespace curl {
namespace detail {

struct curl_delete {
  constexpr curl_delete() noexcept = default;

  void operator()(void *const ptr) const;
};

}  // namespace detail
}  // namespace curl
}  // namespace bunsan
