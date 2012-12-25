#include "bacs/archive/web/content/form/archiver_config.hpp"

namespace bacs{namespace archive{namespace web{namespace content{namespace form
{
    archiver_config::archiver_config()
    {
        type.message(cppcms::locale::translate("Type"));
        format.message(cppcms::locale::translate("Format"));
        add(type);
        add(format);
    }
}}}}}
