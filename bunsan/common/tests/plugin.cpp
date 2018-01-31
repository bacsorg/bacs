#define BOOST_TEST_MODULE plugin
#include <boost/test/unit_test.hpp>

#include <bunsan/plugin.hpp>

#include <boost/dll/runtime_symbol_info.hpp>

BOOST_AUTO_TEST_SUITE(plugin)

class empty_base {
  BUNSAN_PLUGIN_AUTO_BODY(empty_base)
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

BUNSAN_PLUGIN_AUTO_REGISTER(empty_base, empty_concrete,
                            empty_concrete::make_instance)

BOOST_AUTO_TEST_CASE(empty_plugin) {
  auto plugin = empty_base::load_plugin(boost::dll::program_location());
  BOOST_CHECK_EQUAL(plugin.unique_instance()->f(), "empty");
  BOOST_CHECK_EQUAL(plugin.shared_instance()->f(), "empty");
}

class base {
  BUNSAN_PLUGIN_AUTO_BODY(base, const std::string &name)
 public:
  class nested {
    BUNSAN_PLUGIN_AUTO_BODY_NESTED(base, nested, int)
   public:
    virtual ~nested() {}
    virtual int f() = 0;
  };
  BUNSAN_PLUGIN_TYPES(nested)

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

BUNSAN_PLUGIN_AUTO_REGISTER(base, concrete, concrete::make_instance)

class concrete_nested : public base::nested {
 public:
  explicit concrete_nested(const int x) : m_x(x) {}
  int f() override { return -m_x; }
  static base::nested_uptr make_instance(const int x) {
    return std::make_unique<concrete_nested>(x);
  }

 private:
  const int m_x;
};

BUNSAN_PLUGIN_AUTO_REGISTER_NESTED(base, nested, concrete_nested,
                                   concrete_nested::make_instance)

BOOST_AUTO_TEST_CASE(plugin) {
  auto plugin = base::load_plugin(boost::dll::program_location());
  BOOST_CHECK_EQUAL(plugin.unique_instance("hello")->f(), "concrete hello");
  BOOST_CHECK_EQUAL(plugin.shared_instance("world")->f(), "concrete world");
}

BOOST_AUTO_TEST_CASE(shared_plugin) {
  const auto ptr1 = base::load_shared_plugin(boost::dll::program_location());
  const auto ptr2 = ptr1;
  BOOST_CHECK_EQUAL(ptr1->unique_instance("hello")->f(), "concrete hello");
  BOOST_CHECK_EQUAL(ptr2->shared_instance("world")->f(), "concrete world");
}

BOOST_AUTO_TEST_CASE(nested_plugin) {
  auto plugin = base::nested::load_plugin(boost::dll::program_location());
  BOOST_CHECK_EQUAL(plugin.unique_instance(1)->f(), -1);
  BOOST_CHECK_EQUAL(plugin.shared_instance(2)->f(), -2);
}

BOOST_AUTO_TEST_SUITE_END()  // plugin
