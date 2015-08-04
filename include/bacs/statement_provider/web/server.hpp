#pragma once

#include <bacs/statement_provider/request.pb.h>

#include <bunsan/pm/cache.hpp>
#include <bunsan/web/mime_file.hpp>

#include <cppcms/application.h>

#include <boost/filesystem/path.hpp>

#include <memory>

#include <ctime>

namespace bacs {
namespace statement_provider {
namespace web {

class server : public cppcms::application {
 public:
  server(cppcms::service &srv, const std::shared_ptr<bunsan::pm::cache> &cache);

 private:
  void get_index(std::string referrer, std::string getter,
                 std::string coded_request);

  void get(std::string referrer, std::string getter, std::string coded_request,
           std::string path);

  /// \return false if instant return is required (response() was initialized)
  bool get_index_and_root(const std::string &referrer,
                          const std::string &getter,
                          const std::string &coded_request,
                          bunsan::pm::cache::entry *cache_entry = nullptr,
                          boost::filesystem::path *index = nullptr,
                          boost::filesystem::path *data_root = nullptr);

  bool parse(const std::string &referrer, const std::string &getter,
             const std::string &coded_request, Request &request);
  bool parse_binary(const std::string &coded_request, Request &request);
  bool parse_text(const std::string &coded_request, Request &request);

 private:
  const std::shared_ptr<bunsan::pm::cache> m_cache;
  const bunsan::web::mime_file m_mime_file;
};

}  // namespace web
}  // namespace statement_provider
}  // namespace bacs
