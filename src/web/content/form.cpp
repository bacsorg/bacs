#include <bacs/archive/web/content/form.hpp>

namespace bacs {
namespace archive {
namespace web {
namespace content {

status::status() : basic_form(cppcms::locale::translate("Get status")) {}

clear_flags::clear_flags()
    : basic_form(cppcms::locale::translate("Clear flags")) {}

ignore::ignore() : basic_form(cppcms::locale::translate("Ignore")) {}

info::info() : basic_form(cppcms::locale::translate("Get info")) {}

}  // namespace content
}  // namespace web
}  // namespace archive
}  // namespace bacs
