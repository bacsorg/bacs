#define BOOST_TEST_MODULE task
#include <boost/test/unit_test.hpp>

#include <bunsan/broker/task/stream_channel.hpp>

#include <bunsan/crypto/base64.hpp>
#include <bunsan/protobuf/binary.hpp>

#include <sstream>

namespace broker = bunsan::broker;
namespace task = broker::task;

BOOST_AUTO_TEST_SUITE(task_)

struct stream_channel_fixture
{
    std::ostringstream sout;
    task::stream_channel channel{sout};

    template <typename Message>
    Message check_command(const std::string &command)
    {
        BOOST_REQUIRE(!sout.str().empty());
        BOOST_TEST_MESSAGE("[" << sout.str() << "]");
        BOOST_CHECK_EQUAL(sout.str().find('\n'), sout.str().size() - 1);
        std::istringstream sin(sout.str());
        std::string cmd;
        BOOST_CHECK(sin >> cmd);
        BOOST_CHECK_EQUAL(cmd, command);
        std::string data;
        BOOST_CHECK(sin >> data);
        std::string empty;
        BOOST_CHECK(!(sin >> empty));
        return bunsan::protobuf::binary::parse_make<Message>(
            bunsan::crypto::base64::decode(data)
        );
    }
};

BOOST_FIXTURE_TEST_SUITE(stream_channel, stream_channel_fixture)

BOOST_AUTO_TEST_CASE(send_status)
{
    broker::Status status;
    status.set_code(123);
    status.set_reason("test reason");
    status.set_data("test data");
    channel.send_status(status);
    const auto status_ = check_command<broker::Status>("status");
    BOOST_CHECK_EQUAL(status_.code(), status.code());
    BOOST_CHECK_EQUAL(status_.reason(), status.reason());
    BOOST_CHECK_EQUAL(status_.data(), status.data());
}

BOOST_AUTO_TEST_CASE(send_result)
{
    broker::Result result;
    result.set_status(broker::Result::ERROR);
    result.set_reason("test reason");
    result.set_data("test data");
    channel.send_result(result);
    const auto result_ = check_command<broker::Result>("result");
    BOOST_CHECK_EQUAL(result_.status(), result.status());
    BOOST_CHECK_EQUAL(result_.reason(), result.reason());
    BOOST_CHECK_EQUAL(result_.data(), result.data());
}

BOOST_AUTO_TEST_SUITE_END() // stream_channel

BOOST_AUTO_TEST_SUITE_END() // task_
