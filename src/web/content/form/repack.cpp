#include <bacs/archive/web/content/form/repack.hpp>

namespace bacs {
namespace archive {
namespace web {
namespace content {
namespace form {

repack::repack()
    : ids(cppcms::locale::translate("All"),
          widgets::problem::optional_id_set::flag_disables) {
  submit.value(cppcms::locale::translate("Repack"));
  add(ids);
  add(submit);
}

}  // namespace form
}  // namespace content
}  // namespace web
}  // namespace archive
}  // namespace bacs
