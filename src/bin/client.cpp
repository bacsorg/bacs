#include <bacs/archive/archive_client.hpp>
#include <bacs/archive/error.hpp>

#include <bunsan/application.hpp>
#include <bunsan/filesystem/operations.hpp>
#include <bunsan/log/trivial.hpp>
#include <bunsan/protobuf/text.hpp>

#include <grpc++/grpc++.h>

using namespace bacs;
using namespace archive;
using namespace bunsan::application;

namespace {
struct arguments_error : virtual error {};
struct arguments_size_error : virtual arguments_error {
  using expected_size =
      boost::error_info<struct tag_expected_size, std::size_t>;
  using expected_minimum_size =
      boost::error_info<struct tag_expected_minimum_size, std::size_t>;
  using actual_size = boost::error_info<struct tag_actual_size, std::size_t>;
};

class archive_application : public application {
 public:
  using application::application;

  void initialize_argument_parser(argument_parser &parser) override {
    application::initialize_argument_parser(parser);
    parser.add_options()(
        "target", value<std::string>(&target)->required(), "Archive host:port"
    )(
        "ssl", value<bool>(&use_ssl)->default_value(false), "Use SSL"
    )(
        "ssl_pem_root_certs", value<std::string>(&ssl_pem_root_certs),
        "SSL root certificates, leave empty (or skip) to use OS"
    )(
        "ssl_pem_private_key", value<std::string>(&ssl_pem_private_key),
        "SSL private key, leave empty "
    )(
        "ssl_pem_cert_chain", value<std::string>(&ssl_pem_cert_chain),
        "SSL certificate chain"
    );
    parser.add_positional("command", 1,
                          value<std::string>(&command)->required())
        .add_positional("arguments", -1,
                        value<std::vector<std::string>>(&arguments));
  }

  template <typename T>
  static T parse(const std::string &argument) {
    return bunsan::protobuf::text::parse_make<T>(argument);
  }

  void at_least(const std::size_t min) {
    if (arguments.size() < min) {
      BOOST_THROW_EXCEPTION(
          arguments_size_error()
          << arguments_size_error::actual_size(arguments.size())
          << arguments_size_error::expected_minimum_size(min));
    }
  }

  void exactly(const std::size_t size) {
    if (arguments.size() != size) {
      BOOST_THROW_EXCEPTION(
          arguments_size_error()
          << arguments_size_error::actual_size(arguments.size())
          << arguments_size_error::expected_size(size));
    }
  }

  const std::string &arg(const std::size_t index) {
    BOOST_ASSERT(index < arguments.size());
    return arguments[index];
  }

  std::vector<std::string> args(const std::size_t from) {
    BOOST_ASSERT(from <= arguments.size());
    return std::vector<std::string>(arguments.begin() + from, arguments.end());
  }

  int main(const variables_map & /*variables*/) override {
    std::shared_ptr<grpc::Credentials> credentials;
    if (use_ssl) {
      grpc::SslCredentialsOptions ssl;
      if (!ssl_pem_root_certs.empty()) {
        ssl.pem_root_certs = bunsan::filesystem::read_data(ssl_pem_root_certs);
      }
      if (!ssl_pem_private_key.empty()) {
        ssl.pem_private_key =
            bunsan::filesystem::read_data(ssl_pem_private_key);
      }
      if (!ssl_pem_cert_chain.empty()) {
        ssl.pem_cert_chain = bunsan::filesystem::read_data(ssl_pem_cert_chain);
      }
      credentials = grpc::SslCredentials(ssl);
    } else {
      credentials = grpc::InsecureCredentials();
    }
    const auto channel = grpc::CreateChannel(target, credentials);
    ArchiveClient client(channel);
    if (command == "Upload") {
      exactly(2);
      client.Upload(parse<utility::Archiver>(arg(0)), arg(1))
          .PrintDebugString();
    } else if (command == "Download") {
      at_least(3);
      client.Download(parse<utility::Archiver>(arg(0)), arg(1), args(2))
          .PrintDebugString();
    } else if (command == "Rename") {
      exactly(2);
      client.Rename(arg(0), arg(1)).PrintDebugString();
    } else if (command == "Existing") {
      at_least(1);
      client.Existing(args(0)).PrintDebugString();
    } else if (command == "ExistingAll") {
      exactly(0);
      client.ExistingAll().PrintDebugString();
    } else if (command == "Status") {
      at_least(1);
      client.Status(args(0)).PrintDebugString();
    } else if (command == "StatusAll") {
      exactly(0);
      client.StatusAll().PrintDebugString();
    } else if (command == "ImportResult") {
      at_least(1);
      client.ImportResult(args(0)).PrintDebugString();
    } else if (command == "Import") {
      at_least(1);
      client.Import(args(0)).PrintDebugString();
    } else if (command == "ImportAll") {
      exactly(0);
      client.ImportAll().PrintDebugString();
    } else if (command == "WithFlag") {
      // client.WithFlag(arg(0), args(1)).PrintDebugString();
    } else if (command == "WithFlagAll") {
      // client.WithFlagAll(arg(0)).PrintDebugString();
    } else if (command == "SetFlags") {
      // client.SetFlags(parse<ChangeFlagsRequest>(arg(0))).PrintDebugString();
    } else if (command == "UnsetFlags") {
      // client.UnsetFlags(parse<ChangeFlagsRequest>(arg(0))).PrintDebugString();
    } else if (command == "ClearFlags") {
      // client.ClearFlags(args(0)).PrintDebugString();
    }
    return exit_success;
  }

 private:
  std::string target;
  std::string command;
  std::vector<std::string> arguments;
  bool use_ssl;
  std::string ssl_pem_root_certs;
  std::string ssl_pem_private_key;
  std::string ssl_pem_cert_chain;
};
}  // namespace

int main(int argc, char *argv[]) {
  archive_application app(argc, argv);
  app.name("bacs::archive::client");
  return app.exec();
}
