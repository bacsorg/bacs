#pragma once

#include "bacs/archive/problem.hpp"

#include "bunsan/error.hpp"

namespace bacs{namespace archive
{
    struct error: virtual bunsan::error {};

    struct problem_error: virtual error
    {
        typedef boost::error_info<struct tag_problem_id, std::string> problem_id;
    };

    struct format_error: virtual error {};

    struct unknown_archiver_error: virtual error
    {
        typedef boost::error_info<struct tag_format, problem::archiver_config> archiver_config;
    };
}}
