#define BOOST_TEST_MODULE index
#include <boost/test/unit_test.hpp>

#include "bunsan/pm/index.hpp"

#include <sstream>

#include <boost/property_tree/info_parser.hpp>

/// \note ::index is defined by gcc
BOOST_AUTO_TEST_SUITE(index_)

BOOST_AUTO_TEST_CASE(load)
{
    std::stringstream ind(R"EOF(
package
{
    path1 package/number/one
    path2 package/number/two
}

source
{
    self
    {
        path3 source1
        path4 source2
    }

    import
    {
        package
        {
            path5 package/number/three
            path6 package/number/four
        }
        source
        {
            path7 package/number/five
            path8 package/number/six
        }
    }
}
)EOF");
    boost::property_tree::ptree ptree;
    boost::property_tree::read_info(ind, ptree);
    const bunsan::pm::index index(ptree);
    const decltype(index.package) package = {
        {"path1", "package/number/one"},
        {"path2", "package/number/two"}
    };
    const decltype(index.source.self) source_self = {
        {"path3", "source1"},
        {"path4", "source2"}
    };
    const decltype(index.source.import.package) source_import_package = {
        {"path5", "package/number/three"},
        {"path6", "package/number/four"}
    };
    const decltype(index.source.import.source) source_import_source = {
        {"path7", "package/number/five"},
        {"path8", "package/number/six"}
    };
    const std::set<bunsan::pm::entry> all = {
        "package/number/one",
        "package/number/two",
        "package/number/three",
        "package/number/four",
        "package/number/five",
        "package/number/six"
    };
    BOOST_CHECK(index.package == package);
    BOOST_CHECK(index.source.self == source_self);
    BOOST_CHECK(index.source.import.package == source_import_package);
    BOOST_CHECK(index.source.import.source == source_import_source);
    BOOST_CHECK(index.all() == all);
}

BOOST_AUTO_TEST_SUITE_END() // index
