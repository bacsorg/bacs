#include <bacs/archive/web/content/form/with_flag.hpp>

namespace bacs {
namespace archive {
namespace web {
namespace content {
namespace form {

with_flag::with_flag()
    : ids(cppcms::locale::translate("All"),
          widgets::problem::optional_id_set::flag_disables) {
  flag.name("flag");
  flag.message(cppcms::locale::translate("Flag"));
  submit.value(cppcms::locale::translate("Get problems with flag"));
  add(ids);
  add(flag);
  add(submit);
}

}  // namespace form
}  // namespace content
}  // namespace web
}  // namespace archive
}  // namespace bacs
