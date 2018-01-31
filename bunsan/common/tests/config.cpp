#define BOOST_TEST_MODULE config
#include <boost/test/unit_test.hpp>

#include <bunsan/config/cast.hpp>
#include <bunsan/config/input_archive.hpp>
#include <bunsan/config/output_archive.hpp>
#include <bunsan/config/traits.hpp>
#include <bunsan/stream_enum.hpp>

#include <boost/property_tree/ptree.hpp>

struct parent {};
struct derived {};

BUNSAN_CONFIG_EXPORT(::parent, ::derived, "hello, world!")

using variant = boost::variant<std::string, int>;

BUNSAN_CONFIG_EXPORT(variant, std::string, "string")
BUNSAN_CONFIG_EXPORT(variant, int, "int")

BOOST_AUTO_TEST_SUITE(config)

BOOST_AUTO_TEST_CASE(traits) {
  enum my_enum {};
  static_assert(
      bunsan::config::traits::type_key<struct a, struct b>::call() == nullptr,
      "not defined");
  static_assert(bunsan::config::traits::type_key<::parent, ::derived>::call(),
                "defined");
  static_assert(
      bunsan::config::traits::is_direct_assignable<std::string>::value,
      "direct");
  static_assert(bunsan::config::traits::is_direct_assignable<int>::value,
                "direct");
  static_assert(bunsan::config::traits::is_direct_assignable<my_enum>::value,
                "direct");
  BOOST_CHECK_EQUAL(
      (bunsan::config::traits::type_key<::parent, ::derived>::call()),
      "hello, world!");
}

struct fixture {
  struct cfg_type {
    BUNSAN_INCLASS_STREAM_ENUM_CLASS(my_enum, (first, second))

    int a;
    std::string b;
    my_enum c;
    variant d;
    boost::property_tree::ptree e;

    template <typename Archive>
    void serialize(Archive &ar, const unsigned int) {
      ar & BOOST_SERIALIZATION_NVP(a);
      ar & BOOST_SERIALIZATION_NVP(b);
      ar & BOOST_SERIALIZATION_NVP(c);
      ar & BOOST_SERIALIZATION_NVP(d);
      ar & BOOST_SERIALIZATION_NVP(e);
    }
  };

  fixture() {
    tree_int.put("a", 10);
    tree_int.put("b", "hello");
    tree_int.put("c", "second");
    tree_int.put("e.x", "x text");
    tree_int.put("e.y", "y text");
    tree_int.get_child("e").sort();
    tree_multiple = tree_no = tree_other = tree_str = tree_int;
    tree_int.put("d.int", "20");
    tree_str.put("d.string", "string");
    tree_other.put("d.other", "something");
    // we need d present with no (empty) value and no children
    tree_no.put("d", "");
    tree_multiple.put("d.int", 30);
    tree_multiple.put("d.string", "hello");
  }

  boost::property_tree::ptree tree_int, tree_str, tree_other, tree_no,
      tree_multiple;
};

BOOST_FIXTURE_TEST_SUITE(archive, fixture)

BOOST_AUTO_TEST_CASE(input) {
  cfg_type cfg_int, cfg_str, cfg_err;
  bunsan::config::input_archive<boost::property_tree::ptree> in_int(tree_int),
      in_str(tree_str), in_other(tree_other), in_no(tree_no),
      in_multiple(tree_multiple);
  in_int >> cfg_int;
  in_str >> cfg_str;
  BOOST_CHECK_THROW(in_other >> cfg_err,
                    bunsan::config::variant_load_invalid_key_error);
  BOOST_CHECK_THROW(in_no >> cfg_err,
                    bunsan::config::variant_load_no_key_error);
  BOOST_CHECK_THROW(in_multiple >> cfg_err,
                    bunsan::config::variant_load_multiple_keys_error);
  BOOST_CHECK_EQUAL(cfg_int.a, 10);
  BOOST_CHECK_EQUAL(cfg_int.b, "hello");
  BOOST_CHECK_EQUAL(cfg_int.c, cfg_type::my_enum::second);
  BOOST_CHECK(boost::get<int>(&cfg_int.d));
  if (boost::get<int>(&cfg_int.d))
    BOOST_CHECK_EQUAL(*boost::get<int>(&cfg_int.d), 20);
  BOOST_CHECK(boost::get<std::string>(&cfg_str.d));
  if (boost::get<std::string>(&cfg_str.d))
    BOOST_CHECK_EQUAL(*boost::get<std::string>(&cfg_str.d), "string");
  BOOST_CHECK_EQUAL(cfg_int.e.get<std::string>("x"), "x text");
  BOOST_CHECK_EQUAL(cfg_int.e.get<std::string>("y"), "y text");
}

BOOST_AUTO_TEST_CASE(output) {
  cfg_type cfg_int, cfg_str;
  bunsan::config::input_archive<boost::property_tree::ptree> in_int(tree_int),
      in_str(tree_str);
  in_int >> cfg_int;
  in_str >> cfg_str;
  boost::property_tree::ptree otree_int, otree_str;
  bunsan::config::output_archive<boost::property_tree::ptree> out_int(
      otree_int),
      out_str(otree_str);
  out_int << cfg_int;
  out_str << cfg_str;
  otree_int.sort();
  tree_int.sort();
  BOOST_CHECK(otree_int == tree_int);
  otree_str.sort();
  tree_str.sort();
  BOOST_CHECK(otree_str == tree_str);
}

BOOST_AUTO_TEST_CASE(cast) {
  const cfg_type cfg1 = bunsan::config::load<cfg_type>(tree_int);
  bunsan::config::input_archive<boost::property_tree::ptree> in_int(tree_int);
  cfg_type cfg2;
  in_int >> cfg2;
  boost::property_tree::ptree p1 =
      bunsan::config::save<boost::property_tree::ptree>(cfg1);
  boost::property_tree::ptree p2;
  bunsan::config::output_archive<boost::property_tree::ptree> out_int(p2);
  out_int << cfg2;
  p1.sort();
  p2.sort();
  BOOST_CHECK(p1 == p2);
}

BOOST_AUTO_TEST_SUITE_END()  // archive

BOOST_AUTO_TEST_SUITE_END()  // config
