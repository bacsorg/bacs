#define BOOST_TEST_MODULE factory
#include <boost/test/unit_test.hpp>

#include <bunsan/factory.hpp>
#include <bunsan/factory_helper.hpp>

#include <memory>
#include <set>

BOOST_AUTO_TEST_SUITE(factory)

struct check_unknown_factory_error {
  explicit check_unknown_factory_error(const std::string &type_)
      : type(type_) {}

  template <typename T>
  bool operator()(const T &e) {
    const std::string *const ft =
        e.template get<bunsan::unknown_factory_error::factory_type>();
    return ft && *ft == type;
  }

  std::string type;
};

BOOST_AUTO_TEST_CASE(basic) {
  using int_ptr = std::shared_ptr<int>;
  using bunsan_factory = bunsan::factory<int_ptr()>;
  typename bunsan_factory::map_ptr_type map;
  BOOST_CHECK_EXCEPTION(bunsan_factory::instance(map, "strange"),
                        bunsan::unknown_factory_error,
                        check_unknown_factory_error("strange"));
  BOOST_CHECK(!bunsan_factory::instance_optional(map, "strange"));
  BOOST_CHECK(bunsan_factory::register_new(
      map, "zero", []() { return std::make_shared<int>(0); }));
  BOOST_CHECK(!bunsan_factory::register_new(
                  map, "zero", []() { return std::make_shared<int>(-1); }));
  BOOST_CHECK_EXCEPTION(bunsan_factory::factory(map, "strange"),
                        bunsan::unknown_factory_error,
                        check_unknown_factory_error("strange"));
  BOOST_CHECK(!bunsan_factory::factory_optional(map, "strange"));
  const typename bunsan_factory::factory_type zero_fact =
      bunsan_factory::factory(map, "zero");
  BOOST_CHECK_EQUAL(*zero_fact(), 0);
  BOOST_CHECK_EQUAL(*bunsan_factory::instance(map, "zero"), 0);
  BOOST_CHECK(bunsan_factory::register_new(
      map, "one", []() { return std::make_shared<int>(1); }));
  BOOST_CHECK_EQUAL(*bunsan_factory::instance(map, "zero"), 0);
  BOOST_CHECK_EQUAL(*bunsan_factory::instance(map, "one"), 1);
  BOOST_CHECK_EXCEPTION(bunsan_factory::instance(map, ""),
                        bunsan::unknown_factory_error,
                        check_unknown_factory_error(""));
  BOOST_CHECK(!bunsan_factory::instance_optional(map, ""));
  {
    const auto set = boost::copy_range<std::set<std::string>>(
        bunsan_factory::registered(map));
    const std::set<std::string> req = {"zero", "one"};
    BOOST_CHECK(req == set);
  }
}

BOOST_AUTO_TEST_CASE(null) {
  using bunsan_factory = bunsan::factory<std::shared_ptr<int>()>;
  typename bunsan_factory::map_ptr_type map;
  BOOST_CHECK(bunsan_factory::registered(map).empty());
}

BOOST_AUTO_TEST_SUITE(helper)

namespace test {
class base {
 public:
  virtual ~base() {}
  virtual std::string g() = 0;
};

class fact : public base {
  BUNSAN_FACTORY_BODY(fact, const std::string &, const std::string &)
 public:
  virtual std::string f() = 0;
};
BUNSAN_FACTORY_TYPES(fact)

class fact2 {
  BUNSAN_FACTORY_BODY(fact2) public : virtual int code() = 0;
};
BUNSAN_FACTORY_TYPES(fact2)

class fact_derived : public fact {
 public:
  std::string f() override { return "fact_derived::f()"; }

  std::string g() override { return "fact_derived::g()"; }
};
}  // namespace test

namespace test {
BUNSAN_FACTORY_DEFINE(fact)
}  // namespace test

BUNSAN_FACTORY_DEFINE(test::fact2)

namespace test {
namespace facts {
class init : public test::fact {
 public:
  explicit init(const std::string data_, const std::string &data2_)
      : m_data(data_), m_data2(data2_) {}

  std::string f() override { return m_data + m_data2; }

  std::string g() override { return m_data2 + m_data; }

 private:
  std::string m_data, m_data2;
  static const bool factory_reg_hook;
};

const bool init::factory_reg_hook = test::fact::register_new(
    "init", [](const std::string &data_, const std::string data2_) {
      return fact::make_shared<init>(data_, data2_);
    });
}

namespace facts2 {
class init : public test::fact2 {
 public:
  init() {}

  int code() override { return 123; }

 private:
  static const bool factory_reg_hook;
};

const bool init::factory_reg_hook = test::fact2::register_new(
    "init", []() { return fact2::make_shared<init>(); });
}  // namespace facts
}  // namespace test

BOOST_AUTO_TEST_CASE(all) {
  {
    {
      const auto set =
          boost::copy_range<std::set<std::string>>(test::fact::registered());
      const std::set<std::string> req = {"init"};
      BOOST_CHECK(req == set);
    }
    BOOST_CHECK_EXCEPTION(test::fact::factory("not_existing_name"),
                          bunsan::unknown_factory_error,
                          check_unknown_factory_error("not_existing_name"));
    BOOST_CHECK(!test::fact::factory_optional("not_existing_name"));
    {
      const test::fact::factory_type fact = test::fact::factory("init");
      const test::fact_ptr ptr = fact("1", "2");
      BOOST_CHECK(ptr->f() == "12");
      BOOST_CHECK(ptr->g() == "21");
    }
    const test::fact_ptr ptr = test::fact::instance("init", "hello,", " world");
    BOOST_REQUIRE(ptr);
    BOOST_CHECK(ptr->f() == "hello, world");
    BOOST_CHECK(ptr->g() == " worldhello,");
    BOOST_CHECK_EXCEPTION(test::fact::instance("noinit", "hello,", " linux"),
                          bunsan::unknown_factory_error,
                          check_unknown_factory_error("noinit"));
    BOOST_CHECK(!test::fact::instance_optional("noinit", "hello,", " linux"));
  }
  {
    {
      const auto set =
          boost::copy_range<std::set<std::string>>(test::fact2::registered());
      const std::set<std::string> req = {"init"};
      BOOST_CHECK(req == set);
    }
    const test::fact2_ptr ptr = test::fact2::instance("init");
    BOOST_REQUIRE(ptr);
    BOOST_CHECK(ptr->code() == 123);
    BOOST_CHECK_EXCEPTION(test::fact2::instance("noinit"),
                          bunsan::unknown_factory_error,
                          check_unknown_factory_error("noinit"));
    BOOST_CHECK(!test::fact2::instance_optional("noinit"));
  }
  {
    test::fact::shared_ptr<test::fact_derived> derived_ptr =
        test::fact::make_shared<test::fact_derived>();
    test::fact_ptr ptr = derived_ptr;
    BOOST_CHECK_EQUAL(ptr->f(), "fact_derived::f()");
    BOOST_CHECK_EQUAL(ptr->g(), "fact_derived::g()");
  }
}

BOOST_AUTO_TEST_SUITE_END()  // helper

BOOST_AUTO_TEST_SUITE_END()  // factory
