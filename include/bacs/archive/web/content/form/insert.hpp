#pragma once

#include "bacs/archive/web/content/form/base.hpp"
#include "bacs/archive/web/content/form/archiver_config.hpp"

namespace bacs{namespace archive{namespace web{namespace content{namespace form
{
    struct insert: cppcms::form
    {
        insert();

        archiver_config config;
        cppcms::widgets::file archive;
        cppcms::widgets::submit submit;
    };
}}}}}
