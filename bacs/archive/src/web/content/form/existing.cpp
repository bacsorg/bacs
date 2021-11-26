#include <bacs/archive/web/content/form/existing.hpp>

namespace bacs::archive::web::content::form {

existing::existing()
    : ids(cppcms::locale::translate("All"),
          widgets::problem::optional_id_set::flag_disables) {
  submit.value(cppcms::locale::translate("Get existing"));
  add(ids);
  add(submit);
}

}  // namespace bacs::archive::web::content::form
