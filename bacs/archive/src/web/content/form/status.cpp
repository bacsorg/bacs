#include <bacs/archive/web/content/form/status.hpp>

namespace bacs::archive::web::content::form {

status::status()
    : ids(cppcms::locale::translate("All"),
          widgets::problem::optional_id_set::flag_disables) {
  submit.value(cppcms::locale::translate("Get status"));
  add(ids);
  add(submit);
}

}  // namespace bacs::archive::web::content::form
