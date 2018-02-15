#include <bacs/archive/web/content/form/unset_flags.hpp>

namespace bacs::archive::web::content::form {

unset_flags::unset_flags() {
  ids.name("ids");
  ids.message(cppcms::locale::translate("Problem ids"));
  flags.name("flags");
  flags.message(cppcms::locale::translate("Flags"));
  submit.value(cppcms::locale::translate("Unset flags"));
  add(ids);
  add(flags);
  add(submit);
}

}  // namespace bacs::archive::web::content::form
