#define BOOST_TEST_MODULE entry
#include <boost/test/unit_test.hpp>

#include <bunsan/pm/entry.hpp>
#include <bunsan/pm/error.hpp>

#include <boost/property_tree/info_parser.hpp>
#include <boost/property_tree/ptree.hpp>

#include <sstream>

BOOST_AUTO_TEST_SUITE(entry_)

using bunsan::pm::entry;

BOOST_AUTO_TEST_CASE(default_) {
  const entry e;
  BOOST_CHECK(e.empty());
  BOOST_CHECK_THROW(e.name(), bunsan::pm::empty_entry_error);
}

BOOST_AUTO_TEST_CASE(empty) {
  BOOST_CHECK_THROW(entry(""), bunsan::pm::invalid_entry_name_error);
}

BOOST_AUTO_TEST_CASE(absolute) {
  const entry root = "my/package/root";
  BOOST_CHECK_EQUAL(entry("absolute/entry").absolute(root),
                    entry("absolute/entry"));
  BOOST_CHECK_EQUAL(entry().absolute(root), entry());
  BOOST_CHECK_EQUAL(entry("./relative/entry").absolute(root),
                    entry("my/package/root/relative/entry"));
  BOOST_CHECK_EQUAL(entry("./../entry").absolute(root),
                    entry("my/package/entry"));
  BOOST_CHECK_EQUAL(entry("../entry").absolute(root),
                    entry("my/package/entry"));
  BOOST_CHECK_EQUAL(entry("hello/../world").absolute(root), entry("world"));
  BOOST_CHECK_EQUAL(entry("absolute/./entry").absolute(root),
                    entry("absolute/entry"));
  // empty root
  BOOST_CHECK_EQUAL(entry("absolute/entry").absolute(),
                    entry("absolute/entry"));
  BOOST_CHECK_EQUAL(entry().absolute(), entry());
  BOOST_CHECK_EQUAL(entry("./relative/entry").absolute(),
                    entry("relative/entry"));
  // in-place
  entry e("./relative/entry");
  e.make_absolute(root);
  BOOST_CHECK_EQUAL(e, entry("my/package/root/relative/entry"));
}

BOOST_AUTO_TEST_CASE(misc) {
  const std::string name = "some/long/name";
  const entry e(name);
  BOOST_CHECK_EQUAL(e.name(), name);
  BOOST_CHECK_EQUAL(e.location().string(), name);
  BOOST_CHECK_EQUAL(e.remote_resource("/repo", "res"),
                    "/repo/" + name + "/res");
  BOOST_CHECK_EQUAL(e.remote_resource("repo", "res"), "repo/" + name + "/res");
  BOOST_CHECK_EQUAL(e.remote_resource("", "res"), name + "/res");
  BOOST_CHECK_EQUAL(e.remote_resource("repo"), "repo/" + name);
  BOOST_CHECK_EQUAL(e.remote_resource(""), name);
  boost::filesystem::path pname(name);
  BOOST_CHECK_EQUAL(e.local_resource(""), pname);
  BOOST_CHECK_EQUAL(e.local_resource("repo"), "repo" / pname);
  BOOST_CHECK_EQUAL(e.local_resource("/repo"), "/repo" / pname);
  BOOST_CHECK_EQUAL(e.local_resource("/repo", "o_O"), "/repo" / pname / "o_O");
  BOOST_CHECK_EQUAL((entry("123/456") / "789").name(), "123/456/789");
  const entry a("some//long/name");
  BOOST_CHECK_EQUAL(a, e);
  std::stringstream in(R"END(
      some
      {
          long
          {
              name "hello world"
          }
      })END");
  boost::property_tree::ptree pt;
  boost::property_tree::read_info(in, pt);
  BOOST_CHECK_EQUAL(pt.get<std::string>(e.ptree_path()), "hello world");
}

BOOST_AUTO_TEST_SUITE_END()  // entry_
