#include "bacs/archive/web/content/form/base.hpp"

namespace bacs{namespace archive{namespace web{namespace content{namespace form
{
    base::base()
    {
        response.name("response");
        response.message(cppcms::locale::translate("Response"));
        response.add(cppcms::locale::translate("HTML"), "html");
        response.add(cppcms::locale::translate("Protocol Buffer"), "protobuf");
        response.selected_id("html");
        response.non_empty();
        add(response);
    }
}}}}}
