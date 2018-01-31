#define BOOST_TEST_MODULE json
#include <boost/test/unit_test.hpp>

#include <bunsan/web/json.hpp>

#include <bunsan/test/filesystem/tempfile.hpp>
#include <bunsan/test/filesystem/write_data.hpp>

BOOST_FIXTURE_TEST_SUITE(json, bunsan::test::filesystem::tempfile)

BOOST_AUTO_TEST_CASE(load_file) {
  bunsan::test::filesystem::write_data(path, "{ \"hello\" : \"world\" }");
  const cppcms::json::value value = bunsan::web::json::load_file(path);
  cppcms::json::value expected;
  expected["hello"] = "world";
  BOOST_CHECK_EQUAL(value, expected);
}

BOOST_AUTO_TEST_CASE(save_file) {
  cppcms::json::value expected;
  expected["hello"] = "world";
  bunsan::web::json::save_file(expected, path);
  const auto value = bunsan::web::json::load_file(path);
  BOOST_CHECK_EQUAL(value, expected);
}

BOOST_AUTO_TEST_SUITE_END()  // json
