#pragma once

namespace bunsan {
namespace curl {
namespace info {
namespace wrapper {

template <typename T, CURLINFO Info>
struct basic_wrapper {
  using result_type = T;
  static constexpr CURLINFO info = Info;
};

template <typename T, CURLINFO Info>
constexpr CURLINFO basic_wrapper<T, Info>::info;

}  // namespace wrapper
}  // namespace info
}  // namespace curl
}  // namespace bunsan
