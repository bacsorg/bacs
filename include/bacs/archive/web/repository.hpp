#pragma once

#include <cppcms/application.h>

namespace bacs{namespace archive{namespace web
{
    class repository: public cppcms::application
    {
    public:
        explicit repository(cppcms::service &srv);
    };
}}}
