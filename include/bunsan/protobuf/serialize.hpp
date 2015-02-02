#pragma once

#include <bunsan/protobuf/serializer.hpp>

#include <google/protobuf/message.h>

#include <utility>

namespace bunsan{namespace protobuf
{
    template <typename ... Args>
    void serialize_partial(const google::protobuf::Message &message,
                           Args &&...args)
    {
        serializer s;
        s.allow_partial(true);
        s.serialize(message, std::forward<Args>(args)...);
    }

    template <typename ... Args>
    void serialize(const google::protobuf::Message &message,
                   Args &&...args)
    {
        serializer s;
        s.allow_partial(false);
        s.serialize(message, std::forward<Args>(args)...);
    }
}}
