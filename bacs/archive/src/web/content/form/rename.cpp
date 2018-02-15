#include <bacs/archive/web/content/form/rename.hpp>

namespace bacs::archive::web::content::form {

rename::rename() {
  current.name("current_id");
  future.name("future_id");
  current.message(cppcms::locale::translate("Current id"));
  future.message(cppcms::locale::translate("Future id"));
  submit.value(cppcms::locale::translate("Rename"));
  add(current);
  add(future);
  add(submit);
}

}  // namespace bacs::archive::web::content::form
