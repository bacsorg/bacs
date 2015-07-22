#define BOOST_TEST_MODULE plugin
#include <boost/test/unit_test.hpp>

#include <bunsan/plugin.hpp>

#include <boost/dll/runtime_symbol_info.hpp>

BOOST_AUTO_TEST_SUITE(plugin)

class empty_base {
  BUNSAN_PLUGIN_BODY(empty_plugin, empty_base)
 public:
  virtual ~empty_base() {}
  virtual std::string f() = 0;
};
BUNSAN_PLUGIN_TYPES(empty_base)

class empty_concrete : public empty_base {
 public:
  empty_concrete() = default;
  std::string f() override { return "empty"; }
  static empty_base_uptr make_instance() {
    return std::make_unique<empty_concrete>();
  }
};

BUNSAN_PLUGIN_REGISTER(empty_plugin, empty_base, empty_concrete,
                       empty_concrete::make_instance)

BOOST_AUTO_TEST_CASE(empty_plugin) {
  auto plugin = empty_base::load_plugin(boost::dll::program_location());
  BOOST_CHECK_EQUAL(plugin.unique_instance()->f(), "empty");
  BOOST_CHECK_EQUAL(plugin.shared_instance()->f(), "empty");
}

class base {
  BUNSAN_PLUGIN_BODY(base_plugin, base, const std::string &name)
 public:
  virtual ~base() {}
  virtual std::string f() = 0;
};
BUNSAN_PLUGIN_TYPES(base)

class concrete : public base {
 public:
  explicit concrete(const std::string &name) : m_name(name) {}
  std::string f() override { return "concrete " + m_name; }
  static base_uptr make_instance(const std::string &name) {
    return std::make_unique<concrete>(name);
  }

 private:
  std::string m_name;
};

BUNSAN_PLUGIN_REGISTER(base_plugin, base, concrete, concrete::make_instance)

BOOST_AUTO_TEST_CASE(plugin) {
  auto plugin = base::load_plugin(boost::dll::program_location());
  BOOST_CHECK_EQUAL(plugin.unique_instance("hello")->f(), "concrete hello");
  BOOST_CHECK_EQUAL(plugin.shared_instance("world")->f(), "concrete world");
}

BOOST_AUTO_TEST_SUITE_END()  // plugin
