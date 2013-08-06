#pragma once

#include <bacs/archive/web/content/form/base.hpp>
#include <bacs/archive/web/content/form/widgets/archiver_config.hpp>

namespace bacs{namespace archive{namespace web{namespace content{namespace form
{
    struct insert: base
    {
        insert();

        widgets::archiver_config config;
        cppcms::widgets::file archive;
        cppcms::widgets::submit submit;
    };
}}}}}
