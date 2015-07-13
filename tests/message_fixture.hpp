#pragma once

#ifndef IO
#define IO binary
#endif

#include <bunsan/protobuf/detail/testing/message.pb.h>

#include <google/protobuf/text_format.h>

#include <boost/preprocessor/cat.hpp>

namespace bp = bunsan::protobuf;
using namespace bp::detail::testing;

struct message_fixture {
  message_fixture() {
    BOOST_REQUIRE(BOOST_PP_CAT(serialize_partial_, IO)(empty, empty_data));

    only_required.set_required_field("required");
    BOOST_REQUIRE(
        BOOST_PP_CAT(serialize_, IO)(only_required, only_required_data));

    only_optional.set_optional_field("optional");
    BOOST_REQUIRE(BOOST_PP_CAT(serialize_partial_, IO)(only_optional,
                                                       only_optional_data));

    optional.set_required_field("required");
    optional.set_optional_field("optional");
    BOOST_REQUIRE(BOOST_PP_CAT(serialize_, IO)(optional, optional_data));

    single.set_required_field("required");
    single.set_optional_field("optional");
    single.add_repeated_field("repeated");
    BOOST_REQUIRE(BOOST_PP_CAT(serialize_, IO)(single, single_data));

    multiple.set_required_field("required");
    multiple.set_optional_field("optional");
    multiple.add_repeated_field("repeated 1");
    multiple.add_repeated_field("repeated 2");
    BOOST_REQUIRE(BOOST_PP_CAT(serialize_, IO)(multiple, multiple_data));
  }

  bool serialize_partial_binary(const google::protobuf::Message &message,
                                std::string &str) {
    return message.SerializePartialToString(&str);
  }

  bool serialize_binary(const google::protobuf::Message &message,
                        std::string &str) {
    return message.SerializeToString(&str);
  }

  bool serialize_partial_text(const google::protobuf::Message &message,
                              std::string &str) {
    return google::protobuf::TextFormat::PrintToString(message, &str);
  }

  bool serialize_text(const google::protobuf::Message &message,
                      std::string &str) {
    if (!message.IsInitialized()) return false;
    return google::protobuf::TextFormat::PrintToString(message, &str);
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
