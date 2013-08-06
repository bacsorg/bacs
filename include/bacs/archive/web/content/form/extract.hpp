#pragma once

#include <bacs/archive/web/content/form/base.hpp>
#include <bacs/archive/web/content/form/widgets/archiver_config.hpp>
#include <bacs/archive/web/content/form/widgets/problem/id_set.hpp>

namespace bacs{namespace archive{namespace web{namespace content{namespace form
{
    struct extract: base
    {
        extract();

        widgets::archiver_config config;
        widgets::problem::id_set ids;
        cppcms::widgets::submit submit;
    };
}}}}}
