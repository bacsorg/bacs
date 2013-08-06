#pragma once

#include <bacs/archive/problem.hpp>

#include <cppcms/form.h>

namespace bacs{namespace archive{namespace web{namespace content{namespace form{namespace widgets{namespace problem
{
    class id_set: public cppcms::widgets::text
    {
    public:
        archive::problem::id_set value();
        void value(const archive::problem::id_set &id_set_);

        bool validate() override;
    };
}}}}}}}
