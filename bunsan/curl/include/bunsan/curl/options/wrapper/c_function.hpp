#pragma once

#include <bunsan/curl/options/traits.hpp>

#include <curl/curl.h>

namespace bunsan {
namespace curl {
namespace options {
namespace wrapper {

template <typename Signature>
class c_function;

template <typename Result, typename... Args>
class c_function<Result(Args...)> {
 public:
  using retention_policy = retention_policy::by_curl;

  using function_ptr = Result (*)(Args...);

 public:
  c_function() = default;

  explicit c_function(const function_ptr data) : m_data(data) {}

  function_ptr data() const { return m_data; }

 private:
  function_ptr m_data = nullptr;
};

}  // namespace wrapper
}  // namespace options
}  // namespace curl
}  // namespace bunsan
