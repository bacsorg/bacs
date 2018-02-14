#include <bacs/archive/web/content/form/upload.hpp>

namespace bacs {
namespace archive {
namespace web {
namespace content {
namespace form {

upload::upload() {
  archive.name("archive");
  archive.message(cppcms::locale::translate("Archive"));
  submit.value(cppcms::locale::translate("Upload"));
  add(config);
  add(archive);
  add(submit);
}

}  // namespace form
}  // namespace content
}  // namespace web
}  // namespace archive
}  // namespace bacs
