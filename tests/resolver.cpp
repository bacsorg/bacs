#define BOOST_TEST_MODULE resolver
#include <boost/test/unit_test.hpp>

#include "bunsan/utility/resolver.hpp"
#include "bunsan/utility/error.hpp"

#include <sstream>

#include <boost/property_tree/info_parser.hpp>

namespace cfg
{
    const char absolute[] = R"EOF(
        absolute
        {
            foobar /usr/some/foobar
        })EOF";

    const char alias[] = R"EOF(
        alias
        {
            foo foo2
        })EOF";

    const char long_alias[] = R"EOF(
        alias
        {
            foo foo2
            bar bar2
            bar2 bar3
            foo2 foo100500
            foo100500 foo3
        })EOF";

    const char alias_absolute[] = R"EOF(
        alias
        {
            foo foo2
        }
        absolute
        {
            foo2 /usr/some/foo2
        })EOF";

    const char ambigous[] = R"EOF(
        alias
        {
            foo foo2
        }
        absolute
        {
            foo /usr/some/foo
            foo2 /usr/some/foo2
        })EOF";

    const char circular_alias[] = R"EOF(
        alias
        {
            foo foo2
            foo3 foo
            foo2 foo3
        })EOF";

    #if defined(BOOST_POSIX_API)
    const char path[] = R"EOF(
        path
        {
            "" sh
        })EOF";

    const char alias_path[] = R"EOF(
        alias
        {
            shell sh
        }
        path
        {
            "" sh
        })EOF";

    const char shell[] = "sh";
    const char shell_filename[] = "sh";
    #elif defined(BOOST_WINDOWS_API)
    const char path[] = R"EOF(
        path
        {
            "" cmd
        })EOF";

    const char alias_path[] = R"EOF(
        alias
        {
            shell cmd
        }
        path
        {
            "" cmd
        })EOF";

    const char shell[] = "cmd";
    const char shell_filename[] = "cmd.exe";
    #else
    #   error "Unsupported platform."
    #endif

    const char alias_absolute_path[] = R"EOF(
        alias
        {
            foo bar
        }
        absolute
        {
            bar /usr/some/bar
        }
        path
        {
            "" bar
        })EOF";
}

struct fixture
{
    bunsan::utility::resolver res;

    void init(const char *cfg)
    {
        std::istringstream scfg(cfg);
        boost::property_tree::ptree cfg_;
        boost::property_tree::read_info(scfg, cfg_);
        res = bunsan::utility::resolver(cfg_);
    }
};

BOOST_FIXTURE_TEST_SUITE(resolver, fixture)

BOOST_AUTO_TEST_CASE(find_executable)
{
    BOOST_CHECK_EQUAL(res.find_executable(cfg::shell).filename(), cfg::shell_filename);
    init(cfg::absolute);
    BOOST_CHECK_EQUAL(res.find_executable("foobar"), "/usr/some/foobar");
    BOOST_CHECK_EQUAL(res.find_executable("/some/abs"), "/some/abs");
    init(cfg::alias);
    BOOST_CHECK_EQUAL(res.find_executable("foo"), "foo2");
    init(cfg::long_alias);
    BOOST_CHECK_EQUAL(res.find_executable("foo"), "foo3");
    init(cfg::ambigous);
    BOOST_CHECK_EQUAL(res.find_executable("foo"), "/usr/some/foo2");
    init(cfg::alias_absolute);
    BOOST_CHECK_EQUAL(res.find_executable("foo"), "/usr/some/foo2");
    BOOST_CHECK_EQUAL(res.find_executable("foo2"), "/usr/some/foo2");
    init(cfg::circular_alias);
    BOOST_CHECK_THROW(res.find_executable("foo"), bunsan::utility::error);
    init(cfg::path);
    BOOST_CHECK_EQUAL(res.find_executable(cfg::shell).filename(), cfg::shell_filename);
    init(cfg::alias_path);
    BOOST_CHECK_EQUAL(res.find_executable("shell").filename(), cfg::shell_filename);
    init(cfg::alias_absolute_path);
    BOOST_CHECK_EQUAL(res.find_executable("foo"), "/usr/some/bar");
}

BOOST_AUTO_TEST_CASE(find_library)
{
#warning unimplemented find_library
}

BOOST_AUTO_TEST_SUITE_END() // resolver
