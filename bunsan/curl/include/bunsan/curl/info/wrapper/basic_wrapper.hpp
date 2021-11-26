#pragma once

namespace bunsan::curl::info::wrapper {

template <typename T, CURLINFO Info>
struct basic_wrapper {
  using result_type = T;
  static constexpr CURLINFO info = Info;
};

template <typename T, CURLINFO Info>
constexpr CURLINFO basic_wrapper<T, Info>::info;

}  // namespace bunsan::curl::info::wrapper
