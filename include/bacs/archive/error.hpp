#ifndef BACS_ARCHIVE_ERROR_HPP
#define BACS_ARCHIVE_ERROR_HPP

#include "bunsan/error.hpp"

#include "bacs/archive/problem.hpp"

namespace bacs{namespace archive
{
    struct error: virtual bunsan::error {};
    struct problem_error: virtual error
    {
        typedef boost::error_info<struct tag_problem_id, std::string> problem_id;
    };
    struct archive_format_error: virtual error
    {
        typedef boost::error_info<struct tag_format, problem::archive_format> format;
    };
    struct unknown_archiver_error: virtual error {};
}}

#endif //BACS_ARCHIVE_ERROR_HPP

