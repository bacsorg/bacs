#pragma once

#include <bunsan/protobuf/parser.hpp>

#include <google/protobuf/message.h>

#include <utility>

namespace bunsan{namespace protobuf
{
    template <typename ... Args>
    void parse_partial(google::protobuf::Message &message, Args &&...args)
    {
        parser p;
        p.allow_partial(true);
        p.parse(message, std::forward<Args>(args)...);
    }

    template <typename ... Args>
    void parse(google::protobuf::Message &message, Args &&...args)
    {
        parser p;
        p.allow_partial(false);
        p.parse(message, std::forward<Args>(args)...);
    }

    template <typename Message, typename ... Args>
    Message parse_partial_make(Args &&...args)
    {
        Message message;
        parse_partial(message, std::forward<Args>(args)...);
        return message;
    }

    template <typename Message, typename ... Args>
    Message parse_make(Args &&...args)
    {
        Message message;
        parse(message, std::forward<Args>(args)...);
        return message;
    }
}}
