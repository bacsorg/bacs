#define BOOST_TEST_MODULE filesystem
#include <boost/test/unit_test.hpp>

#include <bunsan/filesystem/fstream.hpp>
#include <bunsan/filesystem/operations.hpp>

#include <bunsan/testing/filesystem/tempfile.hpp>
#include <bunsan/testing/filesystem/write_data.hpp>

BOOST_AUTO_TEST_SUITE(filesystem)

namespace fs = bunsan::filesystem;

BOOST_AUTO_TEST_SUITE(operations)

BOOST_AUTO_TEST_CASE(keep_in_root)
{
    const boost::filesystem::path roots[] = {"/home/user", "home/user"};
    for (const boost::filesystem::path &root: roots)
    {
        BOOST_CHECK_EQUAL(fs::keep_in_root("some/path", root), root / "some/path");
        BOOST_CHECK_EQUAL(fs::keep_in_root("/some/path", root), root / "/some/path");
        BOOST_CHECK_EQUAL(fs::keep_in_root("../../../../etc/passwd", root), root / "etc/passwd");
        BOOST_CHECK_EQUAL(fs::keep_in_root("/../../../../etc/passwd", root), root / "/etc/passwd");
    }
}

BOOST_AUTO_TEST_SUITE_END() // operations

BOOST_AUTO_TEST_SUITE(fstream)

BOOST_AUTO_TEST_CASE(failbit)
{
    bunsan::testing::filesystem::tempfile tmp;
    bunsan::testing::filesystem::write_data(tmp.path, "not integer");
    bunsan::filesystem::ifstream fin(tmp.path);
    BUNSAN_FILESYSTEM_FSTREAM_WRAP_BEGIN(fin)
    {
        int x;
        fin >> x;
        BOOST_CHECK(fin.fail());
    }
    BUNSAN_FILESYSTEM_FSTREAM_WRAP_END(fin)
    fin.close();
}

BOOST_AUTO_TEST_SUITE_END() // fstream

BOOST_AUTO_TEST_SUITE_END() // filesystem
