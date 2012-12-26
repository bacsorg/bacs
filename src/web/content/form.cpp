#include "bacs/archive/web/content/form.hpp"

namespace bacs{namespace archive{namespace web{namespace content
{
    existing::existing(): basic_form(cppcms::locale::translate("All"),
                                     cppcms::locale::translate("Get existing problems"),
                                     form::optional_list_query::flag_disables) {}

    available::available(): basic_form(cppcms::locale::translate("All"),
                                       cppcms::locale::translate("Get available problems"),
                                       form::optional_list_query::flag_disables) {}

    status::status(): basic_form(cppcms::locale::translate("Get status")) {}

    clear_flags::clear_flags(): basic_form(cppcms::locale::translate("Clear flags")) {}

    ignore::ignore(): basic_form(cppcms::locale::translate("Ignore")) {}

    info::info(): basic_form(cppcms::locale::translate("Get info")) {}

    hash::hash(): basic_form(cppcms::locale::translate("Get hash")) {}

    repack::repack(): basic_form(cppcms::locale::translate("Repack")) {}
}}}}
