#pragma once

#include "bacs/archive/web/content/form/base.hpp"
#include "bacs/archive/web/content/form/widgets/problem.hpp"

namespace bacs{namespace archive{namespace web{namespace content{namespace form
{
    struct optional_list_query: base
    {
        enum flag_type_enum
        {
            flag_enables,
            flag_disables
        };

        optional_list_query(const std::string &flag_,
                            const std::string &query,
                            const flag_type_enum flag_type_);

        boost::optional<problem::id_set> value();

        flag_type_enum flag_type;

        cppcms::widgets::checkbox flag;
        widgets::problem::id_set ids;
        cppcms::widgets::submit submit;
    };
}}}}}

