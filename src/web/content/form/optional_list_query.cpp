#include "bacs/archive/web/content/form/optional_list_query.hpp"

namespace bacs{namespace archive{namespace web{namespace content{namespace form
{
    optional_list_query::optional_list_query(const std::string &flag_,
                                             const std::string &query,
                                             const flag_type_enum flag_type_):
        flag_type(flag_type_)
    {
        flag.name("flag");
        flag.message(flag_);
        switch (flag_type)
        {
        case flag_enables:
            // TODO
            break;
        case flag_disables:
            // TODO
            break;
        }
        ids.name("ids");
        ids.message(cppcms::locale::translate("Problem ids"));
        submit.value(query);
        add(flag);
        add(ids);
        add(submit);
    }

    boost::optional<problem::id_set> optional_list_query::value()
    {
        switch (flag_type)
        {
        case flag_enables:
            if (flag.value())
                return ids.value();
            break;
        case flag_disables:
            if (!flag.value())
                return ids.value();
            break;
        }
        return boost::optional<problem::id_set>();
    }
}}}}}
