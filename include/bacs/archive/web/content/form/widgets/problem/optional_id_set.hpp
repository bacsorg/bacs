#pragma once

#include "bacs/archive/problem.hpp"
#include "bacs/archive/web/content/form/widgets/problem/id_set.hpp"

#include <cppcms/form.h>

namespace bacs{namespace archive{namespace web{namespace content{namespace form{namespace widgets{namespace problem
{
    class optional_id_set: public cppcms::form
    {
    public:
        enum flag_type_enum
        {
            flag_enables,
            flag_disables
        };

        optional_id_set(const std::string &flag_,
                        const flag_type_enum flag_type_);

        boost::optional<archive::problem::id_set> value();
        void value(const boost::optional<archive::problem::id_set> &id_set_);

        bool validate() override;

    private:
        flag_type_enum flag_type;

        cppcms::widgets::checkbox flag;
        widgets::problem::id_set ids;
    };
}}}}}}}
