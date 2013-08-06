#pragma once

#include <bacs/archive/problem.hpp>

#include <bunsan/error.hpp>

namespace bacs{namespace archive
{
    struct error: virtual bunsan::error {};

    struct protobuf_error: virtual error {};
    struct protobuf_parsing_error: virtual protobuf_error {};
    struct protobuf_serialization_error: virtual protobuf_error {};

    struct problem_error: virtual error
    {
        typedef boost::error_info<struct tag_problem_id, problem::id> problem_id;
    };

    struct format_error: virtual error {};

    struct unknown_archiver_error: virtual error
    {
        typedef boost::error_info<struct tag_archiver_type, std::string> archiver_type;
    };

    namespace problem
    {
        struct error: virtual archive::error {};

        struct invalid_flag_error: virtual error
        {
            typedef boost::error_info<struct tag_flag, problem::flag> flag;
        };
    }
}}
