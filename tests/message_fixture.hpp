#pragma once

#include <bunsan/protobuf/detail/testing/message.pb.h>

namespace bp = bunsan::protobuf;
using namespace bp::detail::testing;

struct message_fixture
{
    message_fixture()
    {
        BOOST_REQUIRE(empty.SerializePartialToString(&empty_data));

        only_required.set_required_field("required");
        BOOST_REQUIRE(only_required.SerializeToString(&only_required_data));

        only_optional.set_optional_field("optional");
        BOOST_REQUIRE(
            only_optional.SerializePartialToString(&only_optional_data)
        );

        optional.set_required_field("required");
        optional.set_optional_field("optional");
        BOOST_REQUIRE(optional.SerializeToString(&optional_data));

        single.set_required_field("required");
        single.set_optional_field("optional");
        single.add_repeated_field("repeated");
        BOOST_REQUIRE(single.SerializeToString(&single_data));

        multiple.set_required_field("required");
        multiple.set_optional_field("optional");
        multiple.add_repeated_field("repeated 1");
        multiple.add_repeated_field("repeated 2");
        BOOST_REQUIRE(multiple.SerializeToString(&multiple_data));
    }

    Message empty;
    Message only_required;
    Message only_optional;
    Message optional;
    Message single;
    Message multiple;

    std::string empty_data;
    std::string only_required_data;
    std::string only_optional_data;
    std::string optional_data;
    std::string single_data;
    std::string multiple_data;
};
