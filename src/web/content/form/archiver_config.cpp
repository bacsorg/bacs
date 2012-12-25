#include "bacs/archive/web/content/form/archiver_config.hpp"

namespace bacs{namespace archive{namespace web{namespace content{namespace form
{
    archiver_config::archiver_config()
    {
        type.message(cppcms::locale::translate("Archive type"));
        format.message(cppcms::locale::translate("Archive format"));
        add(type);
        add(format);
    }
}}}}}
