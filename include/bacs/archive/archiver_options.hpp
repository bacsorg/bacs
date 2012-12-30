#pragma once

#include "bunsan/utility/factory_options.hpp"
#include "bunsan/utility/archiver.hpp"

namespace bacs{namespace archive
{
    struct archiver_options: bunsan::utility::factory_options<bunsan::utility::archiver>
    {
        bunsan::utility::archiver_ptr instance(const bunsan::utility::resolver &resolver) const;
    };
}}
