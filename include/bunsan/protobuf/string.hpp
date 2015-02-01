#pragma once

#include <google/protobuf/message.h>

namespace bunsan{namespace protobuf
{
    void append_partial(const google::protobuf::Message &message,
                        std::string &output);
    void append(const google::protobuf::Message &message,
                std::string &output);

    std::string to_string_partial(const google::protobuf::Message &message);
    std::string to_string(const google::protobuf::Message &message);
}}
