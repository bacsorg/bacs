#include <bacs/archive/archive_service.hpp>
#include <bacs/archive/repository.hpp>
#include <bacs/archive/web/repository.hpp>

#include <bunsan/application.hpp>
#include <bunsan/filesystem/operations.hpp>
#include <bunsan/log/trivial.hpp>
#include <bunsan/protobuf/text.hpp>
#include <bunsan/web/json.hpp>

#include <booster/aio/io_service.h>
#include <cppcms/applications_pool.h>
#include <cppcms/service.h>

#include <grpc++/server.h>
#include <grpc++/server_builder.h>
#include <grpc++/server_credentials.h>

#include <boost/asio/io_service.hpp>
#include <boost/property_tree/info_parser.hpp>
#include <boost/property_tree/ptree.hpp>

#include <memory>
#include <thread>

using namespace bacs::archive;
using namespace bunsan::application;

namespace {
class archive_application : public application {
 public:
  using application::application;

  void initialize_argument_parser(argument_parser &parser) override {
    application::initialize_argument_parser(parser);
    parser.add_options()(
        "configuration,c", value<std::string>(&config_path)->required(),
        "Archive configuration"
    )(
        "rpc-configuration,r", value<std::string>(&rpc_config_path)->required(),
        "Configuration of RPC service"
    )(
        "web-configuration,w", value<std::string>(&web_config_path)->required(),
        "Configuration of Web backend"
    );
  }

  auto make_repository(boost::asio::io_service &executor) {
    BUNSAN_LOG_DEBUG << "Parsing configuration at \"" << config_path << "\"";
    boost::property_tree::read_info(config_path, config);
    return std::make_shared<repository>(executor, config);
  }

  auto make_rpc_service(const std::shared_ptr<repository> &repository) {
    BUNSAN_LOG_DEBUG << "Parsing RPC configuration at \"" << rpc_config_path
                     << "\"";
    rpc_config = bunsan::protobuf::text::parse_make<ArchiveServerConfig>(
        boost::filesystem::path(rpc_config_path));
    return std::make_unique<ArchiveService>(rpc_config.service(), repository);
  }

  auto make_rpc_server(ArchiveService &archive) {
    grpc::ServerBuilder builder;
    std::shared_ptr<grpc::ServerCredentials> credentials;
    if (rpc_config.has_ssl_credentials()) {
      const auto &cred = rpc_config.ssl_credentials();
      grpc::SslServerCredentialsOptions ssl;
      ssl.pem_root_certs =
          bunsan::filesystem::read_data(cred.pem_root_certs_path());
      for (const auto &pair : cred.pem_key_cert_pair()) {
        ssl.pem_key_cert_pairs.push_back(
            grpc::SslServerCredentialsOptions::PemKeyCertPair{
                bunsan::filesystem::read_data(pair.private_key_path()),
                bunsan::filesystem::read_data(pair.cert_chain_path())});
      }
      credentials = grpc::SslServerCredentials(ssl);
    } else {
      credentials = grpc::InsecureServerCredentials();
    }
    builder.AddListeningPort(rpc_config.listen(), credentials);
    builder.RegisterService(&archive);
    BUNSAN_LOG_INFO << "Starting RPC server";
    return builder.BuildAndStart();
  }

  auto make_web_server(const std::shared_ptr<repository> &repository) {
    BUNSAN_LOG_DEBUG << "Parsing Web configuration at \"" << web_config_path
                     << "\"";
    web_config = bunsan::web::json::load_file(web_config_path);
    auto srv = std::make_unique<cppcms::service>(web_config);
    srv->applications_pool().mount(
        cppcms::applications_factory<web::repository>(repository));
    return std::move(srv);
  }

  int main(const variables_map & /*variables*/) override {
    boost::asio::io_service executor;

    const auto repository = make_repository(executor);
    const auto rpc_service = make_rpc_service(repository);
    const auto rpc_server = make_rpc_server(*rpc_service);
    const auto web_server = make_web_server(repository);

    std::thread worker([&executor] { executor.run(); });
    boost::asio::io_service::work work(executor);
    web_server->run();  // handles signals
    rpc_server->Shutdown();
    executor.stop();
    worker.join();
    rpc_server->Wait();
    BUNSAN_LOG_INFO << "Server terminated";
    return exit_success;
  }

 private:
  std::string config_path;
  boost::property_tree::ptree config;

  std::string rpc_config_path;
  ArchiveServerConfig rpc_config;

  std::string web_config_path;
  cppcms::json::value web_config;
};
}  // namespace

int main(int argc, char *argv[]) {
  archive_application app(argc, argv);
  app.name("bacs::archive::server");
  return app.exec();
}
