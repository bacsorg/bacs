#pragma once

#include <bunsan/error.hpp>

#include <system_error>

namespace bunsan {
/// \warning Should not be inherited virtually.
struct categorized_error : virtual error {
  categorized_error() = default;

  explicit categorized_error(const std::error_code &ec);
  categorized_error(const std::error_code &ec, const std::string &what);

  using error_code = boost::error_info<struct tag_error_code, std::error_code>;
  using what_message = boost::error_info<struct tag_what_message, std::string>;
};
}  // namespace bunsan

namespace boost {
std::string to_string(const bunsan::categorized_error::error_code &ec);
}  // namespace boost
