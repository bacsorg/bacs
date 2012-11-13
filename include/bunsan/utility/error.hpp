#pragma once

#include "bunsan/error.hpp"

namespace bunsan{namespace utility
{
    struct error: virtual bunsan::error
    {
        error()=default;
        explicit error(const std::string &message_);
    };
}}
