#pragma once

#include <bacs/statement_provider/request.pb.h>

#include <bunsan/pm/cache.hpp>
#include <bunsan/web/mime_file.hpp>

#include <botan/pk_keys.h>

#include <booster/regex.h>
#include <cppcms/application.h>

#include <boost/filesystem/path.hpp>

#include <memory>

#include <ctime>

namespace bacs::statement_provider::web {

class server : public cppcms::application {
 public:
  server(cppcms::service &srv, const std::shared_ptr<bunsan::pm::cache> &cache);

 private:
  void get_index(std::string signed_request);
  void get(std::string signed_request, std::string path);

  bool get_index_and_root(const std::string &signed_request,
                          bunsan::pm::cache::entry *cache_entry = nullptr,
                          boost::filesystem::path *index = nullptr,
                          boost::filesystem::path *data_root = nullptr);

  bool verify_and_parse(const std::string &signed_request, Request &request);
  bool verify(const std::string &coded_request, const std::string &referrer,
              const std::string &signature);
  bool parse(const std::string &getter, const std::string &coded_request,
             Request &request);
  bool parse_binary(const std::string &coded_request, Request &request);
  bool parse_text(const std::string &coded_request, Request &request);

 private:
  const booster::regex m_request_regex;
  const std::shared_ptr<bunsan::pm::cache> m_cache;
  const bunsan::web::mime_file m_mime_file;
  std::unordered_map<std::string, std::unique_ptr<Botan::Public_Key>> m_keys;
};

}  // namespace bacs::statement_provider::web
