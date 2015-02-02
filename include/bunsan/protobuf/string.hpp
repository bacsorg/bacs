#pragma once

#include <bunsan/protobuf/serializer.hpp>

#include <google/protobuf/message.h>

namespace bunsan{namespace protobuf
{
    inline void append_partial(const google::protobuf::Message &message,
                               std::string &output)
    {
        serializer s;
        s.allow_partial(true);
        s.append(message, output);
    }

    inline void append(const google::protobuf::Message &message,
                       std::string &output)
    {
        serializer s;
        s.allow_partial(false);
        s.append(message, output);
    }

    inline void to_string_partial(const google::protobuf::Message &message,
                                  std::string &output)
    {
        serializer s;
        s.allow_partial(true);
        s.to_string(message, output);
    }

    inline void to_string(const google::protobuf::Message &message,
                          std::string &output)
    {
        serializer s;
        s.allow_partial(false);
        s.to_string(message, output);
    }

    inline std::string to_string_partial(const google::protobuf::Message &message)
    {
        serializer s;
        s.allow_partial(true);
        return s.to_string(message);
    }

    inline std::string to_string(const google::protobuf::Message &message)
    {
        serializer s;
        s.allow_partial(false);
        return s.to_string(message);
    }
}}
