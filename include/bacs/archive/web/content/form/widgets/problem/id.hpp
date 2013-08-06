#pragma once

#include <bacs/archive/problem.hpp>

#include <cppcms/form.h>

namespace bacs{namespace archive{namespace web{namespace content{namespace form{namespace widgets{namespace problem
{
    class id: public cppcms::widgets::text
    {
    public:
        archive::problem::id value();
        void value(const archive::problem::id &id_);

        bool validate() override;
    };
}}}}}}}
