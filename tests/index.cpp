#define BOOST_TEST_MODULE index
#include <boost/test/unit_test.hpp>

#include <bunsan/pm/index.hpp>

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/info_parser.hpp>

#include <sstream>

/// \note ::index is defined by gcc
BOOST_AUTO_TEST_SUITE(index_)

BOOST_AUTO_TEST_CASE(load)
{
    std::stringstream ind(R"EOF(
package
{
    self
    {
        path1 source1
    }

    import
    {
        package
        {
            path2 package/number/one
            path3 package/number/two
        }
        source
        {
            path4 package/number/three
        }
    }
}

source
{
    self
    {
        path5 source2
        path6 source3
    }

    import
    {
        package
        {
            path7 package/number/four
            path8 package/number/five
        }
        source
        {
            path9 package/number/six
            path10 package/number/seven
        }
    }
}
)EOF");
    boost::property_tree::ptree ptree;
    boost::property_tree::read_info(ind, ptree);
    const bunsan::pm::index index(ptree);
    const decltype(index.package.self) package_self = {
        {"path1", "source1"}
    };
    const decltype(index.package.import.package) package_import_package = {
        {"path2", "package/number/one"},
        {"path3", "package/number/two"}
    };
    const decltype(index.package.import.package) package_import_source = {
        {"path4", "package/number/three"},
    };
    const decltype(index.source.self) source_self = {
        {"path5", "source2"},
        {"path6", "source3"}
    };
    const decltype(index.source.import.package) source_import_package = {
        {"path7", "package/number/four"},
        {"path8", "package/number/five"}
    };
    const decltype(index.source.import.source) source_import_source = {
        {"path9", "package/number/six"},
        {"path10", "package/number/seven"}
    };
    const std::set<bunsan::pm::entry> all_package = {
        "package/number/one",
        "package/number/two",
        "package/number/three"
    };
    const std::set<bunsan::pm::entry> all_source = {
        "package/number/four",
        "package/number/five",
        "package/number/six",
        "package/number/seven"
    };
    std::set<bunsan::pm::entry> all = all_package;
    all.insert(all_source.begin(), all_source.end());
    BOOST_CHECK(index.package.self == package_self);
    BOOST_CHECK(index.package.import.package == package_import_package);
    BOOST_CHECK(index.package.import.source == package_import_source);
    BOOST_CHECK(index.source.self == source_self);
    BOOST_CHECK(index.source.import.package == source_import_package);
    BOOST_CHECK(index.source.import.source == source_import_source);
    BOOST_CHECK(index.package.all() == all_package);
    BOOST_CHECK(index.source.all() == all_source);
    BOOST_CHECK(index.all() == all);
}

BOOST_AUTO_TEST_SUITE_END() // index
