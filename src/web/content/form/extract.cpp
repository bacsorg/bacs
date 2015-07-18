#include <bacs/archive/web/content/form/extract.hpp>

namespace bacs {
namespace archive {
namespace web {
namespace content {
namespace form {

extract::extract() {
  ids.name("ids");
  ids.message(cppcms::locale::translate("Problem ids"));
  submit.value(cppcms::locale::translate("Extract"));
  add(config);
  add(ids);
  add(submit);
}

}  // namespace form
}  // namespace content
}  // namespace web
}  // namespace archive
}  // namespace bacs
