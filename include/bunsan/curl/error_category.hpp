#pragma once

#include <system_error>

namespace bunsan{namespace curl
{
    const std::error_category &easy_category() noexcept;
    const std::error_category &multi_category() noexcept;
}}
