#include <bacs/archive/repository.hpp>
#include <bacs/archive/web/repository.hpp>

#include <bunsan/application.hpp>
#include <bunsan/log/trivial.hpp>
#include <bunsan/web/json.hpp>

#include <booster/aio/io_service.h>
#include <cppcms/applications_pool.h>
#include <cppcms/service.h>

#include <boost/asio/io_service.hpp>
#include <boost/property_tree/info_parser.hpp>
#include <boost/property_tree/ptree.hpp>

#include <memory>
#include <thread>

using namespace bunsan::application;

namespace {
class archive_application : public application {
 public:
  using application::application;

  void initialize_argument_parser(argument_parser &parser) override {
    application::initialize_argument_parser(parser);
    parser.add_options()("config,c", value<std::string>(&configuration));
  }

  int main(const variables_map & /*variables*/) override {
    BUNSAN_LOG_DEBUG << "Parsing configuration at \"" << configuration << "\"";
    cppcms::service srv(bunsan::web::json::load_file(configuration));
    const std::string cfg =
        srv.settings().get<std::string>("repository.config");
    BUNSAN_LOG_DEBUG << "Parsing nested configuration at \"" << cfg << "\"";
    boost::property_tree::ptree config;
    boost::property_tree::read_info(cfg, config);
    boost::asio::io_service executor;
    std::thread worker([&executor] { executor.run(); });
    boost::asio::io_service::work work(executor);
    const auto repository =
        std::make_shared<bacs::archive::repository>(executor, config);
    srv.applications_pool().mount(
        cppcms::applications_factory<bacs::archive::web::repository>(
            repository));
    BUNSAN_LOG_INFO << "Starting server";
    srv.run();
    executor.stop();
    worker.join();
    BUNSAN_LOG_INFO << "Server terminated";
    return exit_success;
  }

 private:
  std::string configuration;
};
}  // namespace

int main(int argc, char *argv[]) {
  archive_application app(argc, argv);
  app.name("bacs::archive::server");
  return app.exec();
}
