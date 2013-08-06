#pragma once

#include <bacs/archive/web/content/form/base.hpp>
#include <bacs/archive/web/content/form/widgets/problem/flag.hpp>
#include <bacs/archive/web/content/form/widgets/problem/optional_id_set.hpp>

namespace bacs{namespace archive{namespace web{namespace content{namespace form
{
    struct with_flag: base
    {
        with_flag();

        widgets::problem::optional_id_set ids;
        widgets::problem::flag flag;
        cppcms::widgets::submit submit;
    };
}}}}}

