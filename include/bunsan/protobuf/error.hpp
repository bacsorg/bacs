#pragma once

#include <bunsan/error.hpp>

namespace bunsan{namespace protobuf
{
    struct error: virtual bunsan::error
    {
        typedef boost::error_info<
            struct tag_type_name,
            std::string
        > type_name;
        typedef boost::error_info<
            struct tag_initialization_error_string,
            std::string
        > initialization_error_string;
    };

    struct parse_error: virtual error {};
    struct serialize_error: virtual error {};
}}
