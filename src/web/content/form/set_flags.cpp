#include <bacs/archive/web/content/form/set_flags.hpp>

namespace bacs{namespace archive{namespace web{namespace content{namespace form
{
    set_flags::set_flags()
    {
        ids.name("ids");
        ids.message(cppcms::locale::translate("Problem ids"));
        flags.name("flags");
        flags.message(cppcms::locale::translate("Flags"));
        submit.value(cppcms::locale::translate("Set flags"));
        add(ids);
        add(flags);
        add(submit);
    }
}}}}}
