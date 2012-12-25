#pragma once

#include "bacs/archive/web/content/form/base.hpp"
#include "bacs/archive/web/content/form/widgets/problem.hpp"
#include "bacs/archive/web/content/form/archiver_config.hpp"

namespace bacs{namespace archive{namespace web{namespace content{namespace form
{
    struct extract: cppcms::form
    {
        extract();

        archiver_config config;
        widgets::problem::id_set ids;
        cppcms::widgets::submit submit;
    };
}}}}}
