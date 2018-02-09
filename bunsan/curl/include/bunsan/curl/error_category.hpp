#pragma once

#include <system_error>

namespace bunsan::curl {

const std::error_category &easy_category() noexcept;
const std::error_category &multi_category() noexcept;
const std::error_category &share_category() noexcept;

}  // namespace bunsan::curl
