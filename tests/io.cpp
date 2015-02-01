#define BOOST_TEST_MODULE io
#include <boost/test/unit_test.hpp>

#include "message_fixture.hpp"
#include "stream.hpp"

#include <bunsan/protobuf/comparator.hpp>
#include <bunsan/protobuf/error.hpp>
#include <bunsan/protobuf/io.hpp>

#include <google/protobuf/io/coded_stream.h>

#include <boost/algorithm/string/predicate.hpp>
#include <boost/mpl/list.hpp>

BOOST_FIXTURE_TEST_SUITE(io, message_fixture)

BOOST_AUTO_TEST_SUITE(merge)

using merge_types = boost::mpl::list<google::protobuf::io::CodedInputStream>;

BOOST_AUTO_TEST_CASE_TEMPLATE(merge_partial, Stream, merge_types)
{
    Message msg;
    stream<Stream> ss(only_optional_data);
    ss.merge_partial(msg);
    BOOST_CHECK(bp::equal(msg, only_optional));
}

BOOST_AUTO_TEST_CASE_TEMPLATE(empty, Stream, merge_types)
{
    Message msg;
    stream<Stream> ss(empty_data);
    BOOST_CHECK_THROW(ss.merge(msg), bp::parse_error);
}

BOOST_AUTO_TEST_CASE_TEMPLATE(merge, Stream, merge_types)
{
    Message msg;
    stream<Stream> ss(multiple_data);
    ss.merge(msg);
    BOOST_CHECK(bp::equal(msg, multiple));
}

BOOST_AUTO_TEST_SUITE_END() // merge

BOOST_AUTO_TEST_SUITE(parse)

using parse_types = boost::mpl::list<
    google::protobuf::io::CodedInputStream,
    google::protobuf::io::ZeroCopyInputStream,
    boost::mpl::list<google::protobuf::io::ZeroCopyInputStream, std::size_t>,
    std::string,
    boost::mpl::list<const void *, std::size_t>,
    // TODO file descriptor: int
    std::istream
>;

BOOST_AUTO_TEST_CASE_TEMPLATE(parse, Stream, parse_types)
{
    Message msg;
    stream<Stream> ss(multiple_data);
    ss.parse(msg);
    BOOST_CHECK(bp::equal(msg, multiple));
}

BOOST_AUTO_TEST_CASE_TEMPLATE(parse_partial_empty, Stream, parse_types)
{
    Message msg;
    stream<Stream> ss(empty_data);
    ss.parse_partial(msg);
    BOOST_CHECK(bp::equal(msg, empty));
}

BOOST_AUTO_TEST_CASE_TEMPLATE(parse_partial_only_optional, Stream, parse_types)
{
    Message msg;
    stream<Stream> ss(only_optional_data);
    ss.parse_partial(msg);
    BOOST_CHECK(bp::equal(msg, only_optional));
}

BOOST_AUTO_TEST_CASE_TEMPLATE(parse_partial_return, Stream, parse_types)
{
    stream<Stream> ss(only_optional_data);
    const auto msg = ss.template parse_partial_make<Message>();
    BOOST_CHECK(bp::equal(msg, only_optional));
}

BOOST_AUTO_TEST_CASE_TEMPLATE(parse_return, Stream, parse_types)
{
    stream<Stream> ss(multiple_data);
    const auto msg = ss.template parse_make<Message>();
    BOOST_CHECK(bp::equal(msg, multiple));
}

BOOST_AUTO_TEST_SUITE_END() // parse

BOOST_AUTO_TEST_SUITE(serialize)

using serialize_types = boost::mpl::list<
    google::protobuf::io::CodedOutputStream,
    google::protobuf::io::ZeroCopyOutputStream,
    boost::mpl::list<void *, std::size_t>,
    // TODO file descriptor: int
    std::ostream
>;

BOOST_AUTO_TEST_CASE_TEMPLATE(serialize_partial, Stream, serialize_types)
{
    stream<Stream> ss;
    ss.serialize_partial(empty);
    BOOST_CHECK_EQUAL(ss.data(), empty_data);
}

BOOST_AUTO_TEST_CASE_TEMPLATE(serialize_partial_optional, Stream, serialize_types)
{
    stream<Stream> ss;
    ss.serialize_partial(only_optional);
    BOOST_CHECK_EQUAL(ss.data(), only_optional_data);
}

BOOST_AUTO_TEST_CASE_TEMPLATE(serialize_empty, Stream, serialize_types)
{
    stream<Stream> ss;
    BOOST_CHECK_THROW(ss.serialize(empty), bp::serialize_error);
}

BOOST_AUTO_TEST_CASE_TEMPLATE(serialize, Stream, serialize_types)
{
    stream<Stream> ss;
    ss.serialize(multiple);
    BOOST_CHECK_EQUAL(ss.data(), multiple_data);
}

BOOST_AUTO_TEST_SUITE_END() // serialize

BOOST_AUTO_TEST_SUITE(string)

const std::string prefix = "prefix";

BOOST_AUTO_TEST_CASE(append_partial_empty)
{
    std::string buffer = prefix;
    bp::append_partial(empty, buffer);
    BOOST_CHECK(boost::algorithm::starts_with(buffer, prefix));
    BOOST_CHECK_EQUAL(buffer.substr(prefix.size()), empty_data);
}

BOOST_AUTO_TEST_CASE(append_partial_only_optional)
{
    std::string buffer = prefix;
    bp::append_partial(only_optional, buffer);
    BOOST_CHECK(boost::algorithm::starts_with(buffer, prefix));
    BOOST_CHECK_EQUAL(buffer.substr(prefix.size()), only_optional_data);
}

BOOST_AUTO_TEST_CASE(append_empty)
{
    std::string buffer = prefix;
    BOOST_CHECK_THROW(bp::append(empty, buffer), bp::serialize_error);
}

BOOST_AUTO_TEST_CASE(append)
{
    std::string buffer = prefix;
    bp::append(multiple, buffer);
    BOOST_CHECK(boost::algorithm::starts_with(buffer, prefix));
    BOOST_CHECK_EQUAL(buffer.substr(prefix.size()), multiple_data);
}

BOOST_AUTO_TEST_CASE(to_string_partial)
{
    BOOST_CHECK_EQUAL(bp::to_string_partial(empty), empty_data);
    BOOST_CHECK_EQUAL(bp::to_string_partial(only_required), only_required_data);
    BOOST_CHECK_EQUAL(bp::to_string_partial(only_optional), only_optional_data);
    BOOST_CHECK_EQUAL(bp::to_string_partial(optional), optional_data);
    BOOST_CHECK_EQUAL(bp::to_string_partial(single), single_data);
    BOOST_CHECK_EQUAL(bp::to_string_partial(multiple), multiple_data);
}

BOOST_AUTO_TEST_CASE(to_string)
{
    BOOST_CHECK_THROW(bp::to_string(empty), bp::serialize_error);
    BOOST_CHECK_EQUAL(bp::to_string(only_required), only_required_data);
    BOOST_CHECK_THROW(bp::to_string(only_optional), bp::serialize_error);
    BOOST_CHECK_EQUAL(bp::to_string(optional), optional_data);
    BOOST_CHECK_EQUAL(bp::to_string(single), single_data);
    BOOST_CHECK_EQUAL(bp::to_string(multiple), multiple_data);
}

BOOST_AUTO_TEST_SUITE_END() // string

BOOST_AUTO_TEST_SUITE_END() // io
