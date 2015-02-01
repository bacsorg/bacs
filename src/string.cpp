#include <bunsan/protobuf/string.hpp>

#include "helper.hpp"

#include <bunsan/protobuf/error.hpp>

namespace bunsan{namespace protobuf
{
    void append_partial(const google::protobuf::Message &message,
                        std::string &output)
    {
        BUNSAN_PROTOBUF_OPERATION(
            message,
            AppendPartialToString(&output),
            serialize_error
        );
    }

    void append(const google::protobuf::Message &message,
                std::string &output)
    {
        BUNSAN_PROTOBUF_VERIFY_INITIALIZED(message, serialize_error);
        BUNSAN_PROTOBUF_OPERATION(
            message,
            AppendToString(&output),
            serialize_error
        );
    }

    std::string to_string_partial(const google::protobuf::Message &message)
    {
        std::string string;
        BUNSAN_PROTOBUF_OPERATION(
            message,
            SerializePartialToString(&string),
            serialize_error
        );
        return string;
    }

    std::string to_string(const google::protobuf::Message &message)
    {
        std::string string;
        BUNSAN_PROTOBUF_VERIFY_INITIALIZED(message, serialize_error);
        BUNSAN_PROTOBUF_OPERATION(
            message,
            SerializeToString(&string),
            serialize_error
        );
        return string;
    }
}}
