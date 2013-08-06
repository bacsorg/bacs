#include <bacs/archive/web/content/form/insert.hpp>

namespace bacs{namespace archive{namespace web{namespace content{namespace form
{
    insert::insert()
    {
        archive.name("archive");
        archive.message(cppcms::locale::translate("Archive"));
        submit.value(cppcms::locale::translate("Insert"));
        add(config);
        add(archive);
        add(submit);
    }
}}}}}
