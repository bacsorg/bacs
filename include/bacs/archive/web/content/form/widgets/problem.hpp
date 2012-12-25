#pragma once

#include "bacs/archive/problem.hpp"

#include <cppcms/form.h>

#include <type_traits>

namespace bacs{namespace archive{namespace web{namespace content{namespace form{namespace widgets{namespace problem
{
    class id: public cppcms::widgets::text
    {
    public:
        archive::problem::id value();
        void value(const archive::problem::id &id_);

        bool validate() override;
    };

    class id_set: public cppcms::widgets::text
    {
    public:
        archive::problem::id_set value();
        void value(const archive::problem::id_set &id_set_);

        bool validate() override;
    };

    static_assert(std::is_same<archive::problem::id, archive::problem::flag>::value,
                  "Internal assert to use typedef here.");
    typedef id flag;

    class flag_set: public cppcms::widgets::text
    {
    public:
        archive::problem::flag_set value();
        void value(const archive::problem::flag_set &flag_set_);

        bool validate() override;
    };
}}}}}}}
