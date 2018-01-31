#pragma once

#include <exception>
#include <type_traits>
#include <utility>

namespace bunsan {
namespace curl {
namespace options {
namespace wrapper {

template <typename Traits>
class basic_function {
 public:
  using function_type = typename Traits::function_type;
  using result_type = typename function_type::result_type;

  template <typename Arg, typename... Args>
  explicit basic_function(Arg &&arg, Args &&... args)
      : m_function(std::forward<Arg>(arg), std::forward<Args>(args)...) {}

  decltype(&Traits::static_call) callback() const {
    return &Traits::static_call;
  }

  void *data() const {
    // cURL's interface requires (void *)
    return const_cast<basic_function<Traits> *>(this);
  }

  template <typename... Args>
  result_type call(Args &&... args) const noexcept {
    return call_(static_cast<typename Traits::fail_type *>(nullptr),
                 std::forward<Args>(args)...);
  }

 private:
  template <typename Fail, typename... Args>
  result_type call_(Fail *, Args &&... args) const noexcept {
    try {
      return m_function(std::forward<Args>(args)...);
    } catch (...) {
      static_assert(std::is_same<typename Fail::value_type, result_type>::value,
                    "fail_type be result_type!");
      try {
        return Fail();
      } catch (...) {
        std::terminate();
      }
    }
  }

  template <typename... Args>
  result_type call_(void *, Args &&... args) const noexcept {
    try {
      return m_function(std::forward<Args>(args)...);
    } catch (...) {
      std::terminate();
    }
  }

 private:
  function_type m_function;
};

}  // namespace wrapper
}  // namespace options
}  // namespace curl
}  // namespace bunsan
