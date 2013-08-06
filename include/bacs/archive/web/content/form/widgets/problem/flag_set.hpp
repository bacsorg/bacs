#pragma once

#include <bacs/archive/problem.hpp>

#include <cppcms/form.h>

namespace bacs{namespace archive{namespace web{namespace content{namespace form{namespace widgets{namespace problem
{
    class flag_set: public cppcms::widgets::text
    {
    public:
        archive::problem::flag_set value();
        void value(const archive::problem::flag_set &flag_set_);

        bool validate() override;
    };
}}}}}}}
