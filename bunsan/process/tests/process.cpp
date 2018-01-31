#define BOOST_TEST_MODULE process
#include <boost/test/unit_test.hpp>

#include <bunsan/process/context.hpp>

#include <boost/archive/text_iarchive.hpp>
#include <boost/archive/text_oarchive.hpp>

#include <sstream>

BOOST_AUTO_TEST_SUITE(process)

BOOST_AUTO_TEST_SUITE(context)

BOOST_AUTO_TEST_CASE(exceptions) {
  bunsan::process::context ctx;
  BOOST_CHECK_THROW(ctx.use_path(), bunsan::uninitialized_optional_error);
}

BOOST_AUTO_TEST_CASE(arguments) {
  bunsan::process::context ctx;

  ctx.arguments("0");
  BOOST_CHECK_EQUAL(ctx.arguments()[0], "0");

  ctx.arguments("0", "1", "2");
  BOOST_CHECK_EQUAL(ctx.arguments()[0], "0");
  BOOST_CHECK_EQUAL(ctx.arguments()[1], "1");
  BOOST_CHECK_EQUAL(ctx.arguments()[2], "2");

  {
    std::vector<std::string> v = {"0", "1", "2"};

    ctx.arguments(v);
    BOOST_CHECK_EQUAL(ctx.arguments()[0], "0");
    BOOST_CHECK_EQUAL(ctx.arguments()[1], "1");
    BOOST_CHECK_EQUAL(ctx.arguments()[2], "2");

    ctx.arguments(std::move(v));
    BOOST_CHECK(v.empty());
    BOOST_CHECK_EQUAL(ctx.arguments()[0], "0");
    BOOST_CHECK_EQUAL(ctx.arguments()[1], "1");
    BOOST_CHECK_EQUAL(ctx.arguments()[2], "2");
  }

  {
    typedef std::vector<std::string> vs;
    typedef boost::optional<std::string> os;
    typedef std::vector<os> vos;
    const vs _23 = {"second", "third"};
    const vos _56 = {os("fifth"), os(), os("sixth"), os()};
    const vs result = {"first", "second", "third", "fourth", "fifth", "sixth"};
    ctx.arguments("first", _23, os("fourth"), os(), _56);
    BOOST_CHECK(ctx.arguments() == result);
  }
}

BOOST_AUTO_TEST_CASE(arguments_append) {
  bunsan::process::context ctx;

  ctx.arguments("0");
  BOOST_CHECK_EQUAL(ctx.arguments()[0], "0");

  ctx.arguments_append("1");
  BOOST_CHECK_EQUAL(ctx.arguments()[0], "0");
  BOOST_CHECK_EQUAL(ctx.arguments()[1], "1");

  ctx.arguments_append("2", "3");
  BOOST_CHECK_EQUAL(ctx.arguments()[0], "0");
  BOOST_CHECK_EQUAL(ctx.arguments()[1], "1");
  BOOST_CHECK_EQUAL(ctx.arguments()[2], "2");
  BOOST_CHECK_EQUAL(ctx.arguments()[3], "3");

  ctx.arguments("0");
  BOOST_CHECK_EQUAL(ctx.arguments()[0], "0");

  {
    std::vector<std::string> v = {"1", "2", "3"};
    ctx.arguments_append(v);
    BOOST_CHECK_EQUAL(ctx.arguments()[0], "0");
    BOOST_CHECK_EQUAL(ctx.arguments()[1], "1");
    BOOST_CHECK_EQUAL(ctx.arguments()[2], "2");
    BOOST_CHECK_EQUAL(ctx.arguments()[3], "3");
  }
}

BOOST_AUTO_TEST_SUITE_END()  // context

BOOST_AUTO_TEST_SUITE_END()  // process
