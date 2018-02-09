#pragma once

#include <bunsan/error.hpp>

namespace bunsan::utility {

struct error : virtual bunsan::error {
  error() = default;
  explicit error(const std::string &message_);
};

}  // namespace bunsan::utility
