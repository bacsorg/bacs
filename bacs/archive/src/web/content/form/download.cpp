#include <bacs/archive/web/content/form/download.hpp>

namespace bacs {
namespace archive {
namespace web {
namespace content {
namespace form {

download::download() {
  ids.name("ids");
  ids.message(cppcms::locale::translate("Problem ids"));
  submit.value(cppcms::locale::translate("Download"));
  add(config);
  add(ids);
  add(submit);
}

}  // namespace form
}  // namespace content
}  // namespace web
}  // namespace archive
}  // namespace bacs
