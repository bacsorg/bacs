#include <bacs/archive/web/content/form/upload.hpp>

namespace bacs::archive::web::content::form {

upload::upload() {
  archive.name("archive");
  archive.message(cppcms::locale::translate("Archive"));
  submit.value(cppcms::locale::translate("Upload"));
  add(config);
  add(archive);
  add(submit);
}

}  // namespace bacs::archive::web::content::form
