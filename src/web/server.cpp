#include <bacs/statement_provider/web/server.hpp>

#include <bacs/statement_provider/request.pb.h>

#include <bacs/problem/statement.hpp>

#include <bunsan/filesystem/fstream.hpp>
#include <bunsan/filesystem/operations.hpp>

#include <google/protobuf/text_format.h>

#include <cppcms/base64.h>
#include <cppcms/url_dispatcher.h>
#include <cppcms/url_mapper.h>
#include <cppcms/http_response.h>
#include <cppcms/service.h>

namespace bacs {
namespace statement_provider {
namespace web {

#define BACS_STATEMENT_GETTERS "get|text_get"
#define BACS_STATEMENT_REQUEST \
  "(" BACS_STATEMENT_GETTERS ")/([^/]+)/([^/]+)/([^/]+)"
#define BACS_STATEMENT_REQUEST_CAPTURES 4

server::server(cppcms::service &srv,
               const std::shared_ptr<bunsan::pm::cache> &cache)
    : cppcms::application(srv),
      m_request_regex(BACS_STATEMENT_REQUEST),
      m_cache(cache),
      m_mime_file(bunsan::web::load_mime_file(
          srv.settings().get<std::string>("statement_provider.mime_types"))) {
  dispatcher().assign("/(" BACS_STATEMENT_REQUEST ")", &server::get_index, this,
                      1);
  mapper().assign("get_index", "/{1}");

  dispatcher().assign("/(" BACS_STATEMENT_REQUEST ")/(.*)", &server::get, this,
                      1, 2 + BACS_STATEMENT_REQUEST_CAPTURES);
  mapper().assign("get", "/{1}/{2}");

  mapper().root("/statement");
}

void server::get_index(std::string signed_request) {
  boost::filesystem::path index;
  if (get_index_and_root(signed_request, nullptr, &index)) {
    response().status(cppcms::http::response::temporary_redirect);
    response().set_redirect_header(
        url("get", signed_request, index.generic_string()));
  }
}

void server::get(std::string signed_request, std::string path) {
  bunsan::pm::cache::entry cache_entry;
  boost::filesystem::path data_root;
  if (get_index_and_root(signed_request, &cache_entry, nullptr, &data_root)) {
    const boost::filesystem::path filepath =
        bunsan::filesystem::keep_in_root(path, data_root);
    const std::string filename = filepath.filename().string();
    if (boost::filesystem::is_regular_file(filepath)) {
      response().content_type(m_mime_file.mime_type_by_name(filepath));
      BOOST_ASSERT(filename.find('"') == std::string::npos);
      response().set_header("Content-Disposition",
                            "filename=\"" + filename + "\"");
      response().content_length(boost::filesystem::file_size(filepath));
      // gzip encoding works incorrectly in combination with content length
      response().io_mode(cppcms::http::response::nogzip);
      bunsan::filesystem::ifstream fin(filepath);
      BUNSAN_FILESYSTEM_FSTREAM_WRAP_BEGIN(fin) {
        response().out() << fin.rdbuf();
      } BUNSAN_FILESYSTEM_FSTREAM_WRAP_END(fin)
      fin.close();
    } else {
      response().status(cppcms::http::response::not_found);
    }
  }
}

bool server::get_index_and_root(const std::string &signed_request,
                                bunsan::pm::cache::entry *cache_entry,
                                boost::filesystem::path *const index,
                                boost::filesystem::path *const data_root) {
  Request request;
  if (!verify_and_parse(signed_request, request)) {
    return false;
  }
  bunsan::pm::entry package;
  try {
    package = request.package();
  } catch (bunsan::pm::invalid_entry_name_error &) {
    response().status(cppcms::http::response::bad_request,
                      "Invalid package name");
    return false;
  }
  bunsan::pm::cache::entry entry;
  if (!cache_entry) cache_entry = &entry;
  *cache_entry = m_cache->get(package);
  const boost::filesystem::path package_dir = cache_entry->root();
  const problem::statement::version::built statement_version(package_dir);
  if (request.has_revision() && false) {  // TODO check revision
    response().status(cppcms::http::response::gone,
                      "Problem's revision differs from requested");
    return false;
  }
  if (index) *index = statement_version.manifest().data.index;
  if (data_root) *data_root = statement_version.data_root();
  return true;
}

bool server::verify_and_parse(const std::string &signed_request,
                              Request &request) {
  const char *const begin = signed_request.data();
  const char *const end = begin + signed_request.size();
  std::vector<std::pair<int, int>> marks;
  if (!m_request_regex.match(begin, end, marks)) {
    response().status(cppcms::http::response::bad_request,
                      "Invalid request format, /" BACS_STATEMENT_REQUEST
                      "/ was expected");
    return false;
  }
  BOOST_ASSERT(marks.size() == 5);
  const std::string getter(begin + marks[1].first, begin + marks[1].second);
  const std::string coded_request_64(begin + marks[2].first,
                                    begin + marks[2].second);
  const std::string referrer(begin + marks[3].first, begin + marks[3].second);
  const std::string signature_64(begin + marks[4].first,
                                 begin + marks[4].second);
  std::string coded_request;
  if (!cppcms::b64url::decode(coded_request_64, coded_request)) {
    response().status(cppcms::http::response::bad_request,
                      "Invalid base64url encoded request");
    return false;
  }
  std::string signature;
  if (!cppcms::b64url::decode(signature_64, signature)) {
    response().status(cppcms::http::response::bad_request,
                      "Invalid base64url encoded signature");
    return false;
  }
  if (!verify(coded_request, referrer, signature)) return false;
  return parse(getter, coded_request, request);
}

bool server::verify(const std::string &coded_request,
                    const std::string &referrer,
                    const std::string &signature) {
  // TODO
  return true;
}

bool server::parse(const std::string &getter, const std::string &coded_request,
                   Request &request) {
  if (getter == "get") {
    return parse_binary(coded_request, request);
  } else if (getter == "text_get") {
    return parse_text(coded_request, request);
  } else {
    response().status(cppcms::http::response::bad_request,
                      "Unknown request format");
    return false;
  }
}

bool server::parse_binary(const std::string &coded_request, Request &request) {
  if (!request.ParseFromString(coded_request)) {
    response().status(cppcms::http::response::bad_request,
                      "Invalid protobuf message");
    return false;
  }
  return true;
}

bool server::parse_text(const std::string &coded_request, Request &request) {
  if (!google::protobuf::TextFormat::ParseFromString(coded_request, &request)) {
    response().status(cppcms::http::response::bad_request,
                      "Invalid protobuf message");
    return false;
  }
  return true;
}

}  // namespace web
}  // namespace statement_provider
}  // namespace bacs
