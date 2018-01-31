#pragma once

#include <bunsan/application/argument_parser.hpp>
#include <bunsan/application/global_registry.hpp>

#include <boost/optional.hpp>

#include <functional>
#include <utility>

namespace bunsan {
namespace application {

enum exit_code {
  exit_success = 0,
  argument_parser_failure = 100,
  exit_failure = 200
};

class application {
 public:
  explicit application(const int argc, const char *const argv[]);

  boost::optional<std::string> executable() const;

  const std::string &name() const { return m_name; }

  void name(const std::string &name) { m_name = name; }

  virtual ~application();

  /// Should be implemented by user.
  virtual int main(const variables_map &variables) = 0;

  virtual int exec();

  virtual int argument_parser_error(std::exception &e);

  void print_help();

  virtual void print_synopsis();

  virtual void print_version();

  /// Default implementation provides help and version flags.
  virtual void initialize_argument_parser(argument_parser &parser);

 private:
  const int m_argc;
  const char *const *const m_argv;
  std::string m_name;
  argument_parser m_parser;
  global_registry_ptr m_global_registry;
};

}  // namespace application
}  // namespace bunsan
