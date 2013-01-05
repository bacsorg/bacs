#pragma once

#include "bacs/problem/common.hpp"

#include "bunsan/error.hpp"

namespace bacs{namespace problem
{
    struct error: virtual bunsan::error {};

    struct format_error: virtual error
    {
        typedef boost::error_info<struct tag_format, std::string> format;
    };

    struct unknown_format_error: virtual format_error {};

    struct invalid_id_error: virtual error
    {
        typedef boost::error_info<struct tag_id, problem::id> id;
    };
}}
