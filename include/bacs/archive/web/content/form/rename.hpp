#pragma once

#include "bacs/archive/web/content/form/base.hpp"
#include "bacs/archive/web/content/form/widgets/problem.hpp"

namespace bacs{namespace archive{namespace web{namespace content{namespace form
{
    struct rename: cppcms::form
    {
        rename();

        widgets::problem::id current, future;
        cppcms::widgets::submit submit;
    };
}}}}}
