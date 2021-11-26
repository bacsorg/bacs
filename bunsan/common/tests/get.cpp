#define BOOST_TEST_MODULE get
#include <boost/test/unit_test.hpp>

#include <bunsan/get.hpp>

#define BUNSAN_CHECK_EXCEPTION(OPT, MSG)                                    \
  BOOST_CHECK_EXCEPTION(bunsan::get(OPT, MSG),                              \
                        bunsan::uninitialized_optional_error,               \
                        [](const bunsan::uninitialized_optional_error &e) { \
                          return *e.get<bunsan::error::message>() == MSG;   \
                        })

BOOST_AUTO_TEST_SUITE(get)

BOOST_AUTO_TEST_CASE(optional) {
  boost::optional<int> o;
  BUNSAN_CHECK_EXCEPTION(o, "adsdkmas wkdmnakjndjkasnjd");
  o = 10;
  BOOST_CHECK_EQUAL(bunsan::get(o, "dnjasndkasndjknasjkdn"), 10);
}

BOOST_AUTO_TEST_CASE(ptree) {
  boost::property_tree::ptree pt, vl, vl2;
  vl.put_value("231dasmnw");
  vl2.put_value("njskdnjsdnjkasd");
  BOOST_CHECK(bunsan::ptree_get(pt, "some.path", vl) == vl);
  BOOST_CHECK(pt.get_child("some.path") == vl);
  BOOST_CHECK(bunsan::ptree_get(pt, "some.path", vl2) == vl);
  bunsan::ptree_get(pt, "some.path").put("name0", "value0");
  bunsan::ptree_get(pt, "some.path").put("name1", "value1");
  BOOST_CHECK_EQUAL(pt.get<std::string>("some.path.name0"), "value0");
  BOOST_CHECK_EQUAL(pt.get<std::string>("some.path.name1"), "value1");
}

BOOST_AUTO_TEST_SUITE_END()  // get
