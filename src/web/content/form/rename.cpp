#include "bacs/archive/web/content/form/rename.hpp"

namespace bacs{namespace archive{namespace web{namespace content{namespace form
{
    rename::rename()
    {
        current.message(cppcms::locale::translate("Current id"));
        future.message(cppcms::locale::translate("Future id"));
        submit.message(cppcms::locale::translate("Rename"));
        add(current);
        add(future);
        add(submit);
    }
}}}}}
