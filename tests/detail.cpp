#define BOOST_TEST_MODULE algorithm
#include <boost/test/unit_test.hpp>

#include "bunsan/utility/detail/join.hpp"

#include <string>

BOOST_AUTO_TEST_SUITE(detail)

BOOST_AUTO_TEST_CASE(join)
{
    using bunsan::utility::detail::join;
    typedef std::vector<std::string> vs;
    typedef boost::optional<std::string> os;
    typedef std::vector<os> vos;
    const vs _23 = {"second", "third"};
    const vos _56 = {os("fifth"), os(), os("sixth"), os()};
    const vs result = {"first", "second", "third", "fourth", "fifth", "sixth"};
    BOOST_CHECK(join<std::string>("first", _23, os("fourth"), os(), _56) == result);
}

BOOST_AUTO_TEST_SUITE_END() // algorithm
