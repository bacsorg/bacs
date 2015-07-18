#include <bacs/archive/web/repository.hpp>

#include <bacs/archive/error.hpp>
#include <bacs/archive/web/content/error.hpp>
#include <bacs/archive/web/content/form.hpp>

#include <bunsan/enable_error_info.hpp>
#include <bunsan/filesystem/fstream.hpp>

#include <cppcms/http_file.h>
#include <cppcms/service.h>
#include <cppcms/url_dispatcher.h>
#include <cppcms/url_mapper.h>

#include <boost/assert.hpp>
#include <boost/filesystem/operations.hpp>

#include <functional>

namespace bacs {
namespace archive {
namespace web {

repository::repository(cppcms::service &srv,
                       const std::shared_ptr<archive::repository> &repository_)
    : cppcms::application(srv),
      m_repository(repository_),
      m_upload_directory(
          srv.settings().get<std::string>("repository.upload_directory")) {
  dispatcher().assign("/insert", &repository::insert, this);
  mapper().assign("insert", "/insert");

  dispatcher().assign("/extract", &repository::extract, this);
  mapper().assign("extract", "/extract");

  dispatcher().assign("/rename", &repository::rename, this);
  mapper().assign("rename", "/rename");

  dispatcher().assign("/existing", &repository::existing, this);
  mapper().assign("existing", "/existing");

  dispatcher().assign("/available", &repository::available, this);
  mapper().assign("available", "/available");

  dispatcher().assign("/status", &repository::status, this);
  mapper().assign("status", "/status");

  dispatcher().assign("/with_flag", &repository::with_flag, this);
  mapper().assign("with_flag", "/with_flag");

  dispatcher().assign("/set_flags", &repository::set_flags, this);
  mapper().assign("set_flags", "/set_flags");

  dispatcher().assign("/unset_flags", &repository::unset_flags, this);
  mapper().assign("unset_flags", "/unset_flags");

  dispatcher().assign("/clear_flags", &repository::clear_flags, this);
  mapper().assign("clear_flags", "/clear_flags");

  dispatcher().assign("/ignore", &repository::ignore, this);
  mapper().assign("ignore", "/ignore");

  dispatcher().assign("/info", &repository::info, this);
  mapper().assign("info", "/info");

  dispatcher().assign("/hash", &repository::hash, this);
  mapper().assign("hash", "/hash");

  dispatcher().assign("/repack", &repository::repack, this);
  mapper().assign("repack", "/repack");

  mapper().root("/repository");
}

template <typename Model, typename Handler, typename Sender>
void repository::handler_wrapper(const std::string &name,
                                 const Handler &handler, const Sender &sender) {
  Model data;
  if (request().request_method() == "POST") {
    data.form.load(context());
    if (data.form.validate()) {
      bool ok = false;
      try {
        handler(data);
        ok = true;
      } catch (std::exception &e) {
        content::error error;
        switch (data.form.response()) {
          case content::form::base::response_type::html:
            error.brief = translate("Oops! Exception occurred.");
            error.message = e.what();
            render("error", error);
            break;
          case content::form::base::response_type::protobuf:
            response().status(cppcms::http::response::internal_server_error,
                              "Exception occurred");
            response().set_plain_text_header();
            response().out() << e.what();
            break;
        }
      }
      if (ok) sender(data);
    } else {
      switch (data.form.response()) {
        case content::form::base::response_type::html:
          // form::validate() results error messages set for form parts
          render(name, data);
          break;
        case content::form::base::response_type::protobuf: {
          // FIXME Is it correct to use html here?
          response().status(cppcms::http::response::internal_server_error,
                            "Invalid request");
          content::error error;
          error.brief = translate("Invalid request.");
          error.message = translate("Form was not filled correctly.");
          render("error", error);
        } break;
      }
    }
  } else {
    // render empty form
    render(name, data);
  }
}

void repository::main(std::string url) {
  if (!dispatcher().dispatch(url)) {
    response().status(cppcms::http::response::not_found);
    content::error error;
    error.brief = translate("Page not found.");
    error.message =
        translate("Requested page was not found. Try to use menu above.");
    render("error", error);
  }
}

#define DEFINE_HANDLER(NAME, RESULT)                              \
  void repository::NAME() {                                       \
    handler_wrapper<content::NAME>(#NAME,                         \
                                   [this](content::NAME & data) { \
      data.result = repository::NAME##_(data);                    \
                                   },                             \
                                   [this](content::NAME & data) { \
      switch (data.form.response()) {                             \
        case content::form::base::response_type::html:            \
          render(#NAME, data);                                    \
          break;                                                  \
        case content::form::base::response_type::protobuf:        \
          send_protobuf(*data.result, #NAME ".pb.data");          \
          break;                                                  \
      }                                                           \
                                   });                            \
  }                                                               \
  RESULT repository::NAME##_(content::NAME &data)

DEFINE_HANDLER(insert, problem::ImportMap) {
  const bunsan::tempfile tmpfile =
      bunsan::tempfile::regular_file_in_directory(m_upload_directory);
  data.form.archive.value()->save_to(tmpfile.string());
  return m_repository->insert_all(data.form.config.value(), tmpfile.path());
}

/*void repository::extract() {
  content::extract data;
  if (request().request_method() == "POST") {
    data.form.load(context());
    if (data.form.validate()) {
      bunsan::tempfile tmpfile = m_repository->extract_all(
          data.form.ids.value(), data.form.config.value());
      const std::string filename = "archive." + data.form.config.type.value() +
                                   "." + data.form.config.format.value();
      send_tempfile(std::move(tmpfile), filename);
    }
  }
  render("extract", data);
}*/

void repository::extract() {
  bunsan::tempfile tmpfile;
  std::string filename;
  const auto handler = [this, &tmpfile, &filename](content::extract &data) {
    tmpfile = m_repository->extract_all(data.form.ids.value(),
                                        data.form.config.value());
    filename = "archive." + data.form.config.type.value();
    if (!data.form.config.format.value().empty())
      filename.append("." + data.form.config.format.value());
  };
  const auto sender = [this, &tmpfile, &filename](content::extract & /*data*/) {
    send_tempfile(std::move(tmpfile), filename);
  };
  handler_wrapper<content::extract>(__func__, handler, sender);
}

DEFINE_HANDLER(rename, problem::ImportInfo) {
  return m_repository->rename(data.form.current.value(),
                              data.form.future.value());
}

DEFINE_HANDLER(existing, problem::IdSet) {
  const boost::optional<problem::IdSet> ids = data.form.ids.value();
  if (ids) {
    return m_repository->existing(ids.get());
  } else {
    return m_repository->existing();
  }
}

DEFINE_HANDLER(available, problem::IdSet) {
  const boost::optional<problem::IdSet> ids = data.form.ids.value();
  if (ids) {
    return m_repository->available(ids.get());
  } else {
    return m_repository->available();
  }
}

DEFINE_HANDLER(status, problem::ImportMap) {
  return m_repository->status_all(data.form.ids.value());
}

DEFINE_HANDLER(with_flag, problem::IdSet) {
  const boost::optional<problem::IdSet> ids = data.form.ids.value();
  if (ids) {
    return m_repository->with_flag(ids.get(), data.form.flag.value());
  } else {
    return m_repository->with_flag(data.form.flag.value());
  }
}

DEFINE_HANDLER(set_flags, problem::IdSet) {
  return m_repository->set_flags_all(data.form.ids.value(),
                                     data.form.flags.value());
}

DEFINE_HANDLER(unset_flags, problem::IdSet) {
  return m_repository->unset_flags_all(data.form.ids.value(),
                                       data.form.flags.value());
}

DEFINE_HANDLER(clear_flags, problem::IdSet) {
  return m_repository->clear_flags_all(data.form.ids.value());
}

DEFINE_HANDLER(ignore, problem::IdSet) {
  return m_repository->ignore_all(data.form.ids.value());
}

DEFINE_HANDLER(info, problem::InfoMap) {
  return m_repository->info_all(data.form.ids.value());
}

DEFINE_HANDLER(hash, problem::HashMap) {
  return m_repository->hash_all(data.form.ids.value());
}

DEFINE_HANDLER(repack, problem::ImportMap) {
  return m_repository->repack_all(data.form.ids.value());
}

template <typename ProtoBuf>
void repository::send_protobuf(const ProtoBuf &protobuf,
                               const std::string &filename) {
  response().content_type("application/octet-stream");
  BOOST_ASSERT(filename.find('"') == std::string::npos);
  response().set_header("Content-Disposition", "filename=\"" + filename + "\"");
  response().content_length(protobuf.ByteSize());
  if (!protobuf.SerializeToOstream(&response().out()))
    BOOST_THROW_EXCEPTION(protobuf_serialization_error());
}

// FIXME note: these implementations are workarounds,
// use xsendfile and xsendtempfile instead
void repository::send_file(const boost::filesystem::path &path,
                           const std::string &filename) {
  response().content_type("application/octet-stream");
  BOOST_ASSERT(filename.find('"') == std::string::npos);
  response().set_header("Content-Disposition", "filename=\"" + filename + "\"");
  response().content_length(boost::filesystem::file_size(path));
  bunsan::filesystem::ifstream fin(path, std::ios_base::binary);
  BUNSAN_FILESYSTEM_FSTREAM_WRAP_BEGIN(fin) {
    // FIXME be careful about large files
    response().out() << fin.rdbuf();
  } BUNSAN_FILESYSTEM_FSTREAM_WRAP_END(fin)
  fin.close();
}

void repository::send_tempfile(bunsan::tempfile &&tmpfile,
                               const std::string &filename) {
  // FIXME be careful about large files
  send_file(tmpfile.path(), filename);
}

}  // namespace web
}  // namespace archive
}  // namespace bacs
