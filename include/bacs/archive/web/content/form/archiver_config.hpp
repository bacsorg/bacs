#pragma once

#include <cppcms/form.h>

namespace bacs{namespace archive{namespace web{namespace content{namespace form
{
    struct archiver_config: cppcms::form
    {
        archiver_config();

        cppcms::widgets::text type;
        cppcms::widgets::text format;
    };
}}}}}
