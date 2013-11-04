#pragma once

#include <bunsan/error.hpp>
#include <bunsan/filesystem/error.hpp>

#include <boost/filesystem/path.hpp>

namespace bacs{namespace problem
{
    struct error: virtual bunsan::error
    {
        typedef bunsan::filesystem::error::path path;
    };
}}
