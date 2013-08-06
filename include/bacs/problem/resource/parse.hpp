#pragma once

#include <bacs/problem/resource/error.hpp>

#include <string>

namespace bacs{namespace problem{namespace resource{namespace parse
{
    struct error: virtual resource::error
    {
        typedef boost::error_info<struct tag_value, std::string> value;
        typedef boost::error_info<struct tag_pos, std::size_t> pos;
    };

    struct time_error: virtual error {};
    struct memory_error: virtual error {};

    std::uint64_t time_millis(const std::string &time);
    std::uint64_t memory_bytes(const std::string &memory);
}}}}
