#include <bacs/archive/web/content/form/download.hpp>

namespace bacs::archive::web::content::form {

download::download() {
  ids.name("ids");
  ids.message(cppcms::locale::translate("Problem ids"));
  submit.value(cppcms::locale::translate("Download"));
  add(config);
  add(ids);
  add(submit);
}

}  // namespace bacs::archive::web::content::form
