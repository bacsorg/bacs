#pragma once

#include <bunsan/protobuf/parser.hpp>

#include <google/protobuf/message.h>

#include <utility>

namespace bunsan{namespace protobuf
{
    template <typename ... Args>
    void merge_partial(google::protobuf::Message &message, Args &&...args)
    {
        parser p;
        p.allow_partial(true);
        p.merge(message, std::forward<Args>(args)...);
    }

    template <typename ... Args>
    void merge(google::protobuf::Message &message, Args &&...args)
    {
        parser p;
        p.allow_partial(false);
        p.merge(message, std::forward<Args>(args)...);
    }
}}
