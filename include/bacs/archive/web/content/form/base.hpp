#pragma once

#include <cppcms/form.h>

namespace bacs{namespace archive{namespace web{namespace content{namespace form
{
    struct base: cppcms::form
    {
        base();

        cppcms::widgets::radio response;
    };
}}}}}
