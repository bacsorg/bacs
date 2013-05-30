#pragma once

#include "bacs/problem/common.hpp"

#include "bunsan/error.hpp"

#include <boost/filesystem/path.hpp>

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

    struct statement_error: virtual error {};

    struct invalid_statement_lang_error: virtual invalid_id_error, virtual statement_error
    {
        typedef boost::error_info<struct tag_lang, std::string> lang;
    };

    struct invalid_statement_format_error: virtual invalid_id_error, virtual statement_error
    {
        typedef boost::error_info<struct tag_format, std::string> format;
    };

    struct utility_error: virtual error {};

    struct invalid_target_error: virtual utility_error
    {
        typedef boost::error_info<struct tag_target, boost::filesystem::path> target;
    };
}}
