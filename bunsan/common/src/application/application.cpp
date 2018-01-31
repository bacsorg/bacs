#include <bunsan/application/application.hpp>

#include <bunsan/log/trivial.hpp>
#include <bunsan/runtime/demangle.hpp>

#include <boost/assert.hpp>

#include <iostream>

namespace bunsan {
namespace application {

application::application(const int argc, const char *const argv[])
    : m_argc(argc),
      m_argv(argv),
      m_parser("Options"),
      m_global_registry(global_registry::lock()) {
  BOOST_ASSERT(m_argv);
  BOOST_ASSERT(!m_argv[m_argc]);
}

boost::optional<std::string> application::executable() const {
  if (m_argv[0]) return std::string(m_argv[0]);
  return boost::none;
}

application::~application() {}

int application::exec() {
  try {
    initialize_argument_parser(m_parser);
    const variables_map variables = m_parser.parse_command_line(m_argc, m_argv);
    if (variables.count("help")) {
      print_help();
      return exit_success;
    } else if (variables.count("version")) {
      print_version();
      return exit_success;
    }
    return main(variables);
  } catch (boost::program_options::error &e) {
    return argument_parser_error(e);
  } catch (std::exception &e) {
    BUNSAN_LOG_ERROR << "Error of type [" << runtime::type_name(e) << "]\n"
                     << e.what();
    return exit_failure;
  }
}

int application::argument_parser_error(std::exception &e) {
  BUNSAN_LOG_ERROR << "Argument parser error: " << e.what();
  print_help();
  return argument_parser_failure;
}

void application::print_help() {
  const auto exe = executable();
  if (m_name.empty())
    std::cerr << "Usage\n";
  else
    std::cerr << m_name << " usage\n";
  print_synopsis();
  std::cerr << m_parser.help() << std::flush;
}

void application::print_synopsis() {}

void application::print_version() {}

void application::initialize_argument_parser(argument_parser &parser) {
  parser.add_options()("help,h", "Print help")("version", "Print version");
}

}  // namespace application
}  // namespace bunsan
