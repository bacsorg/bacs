#define BOOST_TEST_MODULE application
#include <boost/test/unit_test.hpp>

#include <bunsan/application.hpp>

#include <boost/optional/optional_io.hpp>

namespace b = bunsan;
namespace ba = bunsan::application;

BOOST_AUTO_TEST_SUITE(application)

BOOST_AUTO_TEST_SUITE(application)

struct reg_mock {
  static const bool registered;
  static int ctor;
  static int dtor;

  reg_mock() { ++ctor; }
  ~reg_mock() { ++dtor; }

  static void reset() { ctor = dtor = 0; }
};

const bool reg_mock::registered =
    ba::global_registry::register_unique_state_factory<reg_mock>();
int reg_mock::ctor = 0;
int reg_mock::dtor = 0;

class myapplication : public ba::application {
 public:
  using ba::application::application;

  void initialize_argument_parser(ba::argument_parser &parser) {
    parser.add_options()("option1", "help 1")(
        "option2", ba::value<std::string>(&option2), "help 2");
    parser.add_positional("positional", 1, ba::value<std::string>(&positional),
                          "help positional");
  }

  int main(const ba::variables_map &vm) override {
    variables = vm;
    return ba::exit_success;
  }

  int argument_parser_error(std::exception &) override {
    called_argument_parser_error = true;
    return 12;
  }

  bool called_argument_parser_error = false;
  ba::variables_map variables;
  std::string option2;
  std::string positional;
};

BOOST_AUTO_TEST_CASE(empty_executable) {
  const char *const argv[] = {nullptr};
  myapplication app(sizeof(argv) / sizeof(argv[0]) - 1, argv);
  BOOST_CHECK_EQUAL(app.executable(), boost::none);
}

BOOST_AUTO_TEST_CASE(parser) {
  const char *const argv[] = {"/bin/ls", "--option2=option2-value", "arg1",
                              nullptr};
  myapplication app(sizeof(argv) / sizeof(argv[0]) - 1, argv);
  BOOST_CHECK_EQUAL(app.executable(), std::string("/bin/ls"));
  BOOST_CHECK_EQUAL(app.exec(), 0);
  BOOST_CHECK_EQUAL(app.variables.count("option1"), 0);
  BOOST_CHECK_EQUAL(app.variables.count("option2"), 1);
  BOOST_CHECK_EQUAL(app.option2, "option2-value");
  BOOST_CHECK_EQUAL(app.variables.count("positional"), 1);
  BOOST_CHECK_EQUAL(app.positional, "arg1");
}

BOOST_AUTO_TEST_CASE(argument_parser_failure) {
  const char *const argv[] = {"/bin/ls", "--option2=option2-value", "arg1",
                              "arg2", nullptr};
  myapplication app(sizeof(argv) / sizeof(argv[0]) - 1, argv);
  BOOST_CHECK_EQUAL(app.exec(), 12);
  BOOST_CHECK(app.called_argument_parser_error);
}

BOOST_AUTO_TEST_CASE(global_registry) {
  reg_mock::reset();
  BOOST_CHECK_EQUAL(reg_mock::ctor, 0);
  BOOST_CHECK_EQUAL(reg_mock::dtor, 0);
  {
    const char *const argv[] = {"executable", nullptr};
    myapplication app(1, argv);
    BOOST_CHECK_EQUAL(reg_mock::ctor, 1);
    BOOST_CHECK_EQUAL(reg_mock::dtor, 0);
  }
  BOOST_CHECK_EQUAL(reg_mock::ctor, 1);
  BOOST_CHECK_EQUAL(reg_mock::dtor, 1);
}

BOOST_AUTO_TEST_SUITE_END()  // application

BOOST_AUTO_TEST_SUITE_END()  // application
