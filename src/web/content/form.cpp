#include <bacs/archive/web/content/form.hpp>

namespace bacs {
namespace archive {
namespace web {
namespace content {

clear_flags::clear_flags()
    : basic_form(cppcms::locale::translate("Clear flags")) {}

ignore::ignore() : basic_form(cppcms::locale::translate("Ignore")) {}

get_import_result::get_import_result()
    : basic_form(cppcms::locale::translate("Get import result")) {}

}  // namespace content
}  // namespace web
}  // namespace archive
}  // namespace bacs
