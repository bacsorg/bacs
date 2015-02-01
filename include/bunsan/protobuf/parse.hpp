#pragma once

#include <google/protobuf/message.h>

#include <utility>

namespace bunsan{namespace protobuf
{
    void parse_partial(google::protobuf::Message &message,
                       google::protobuf::io::CodedInputStream &input);
    void parse(google::protobuf::Message &message,
               google::protobuf::io::CodedInputStream &input);

    void parse_partial(google::protobuf::Message &message,
                       google::protobuf::io::ZeroCopyInputStream &input);
    void parse(google::protobuf::Message &message,
               google::protobuf::io::ZeroCopyInputStream &input);

    void parse_partial(google::protobuf::Message &message,
                       google::protobuf::io::ZeroCopyInputStream &input,
                       std::size_t size);
    void parse(google::protobuf::Message &message,
               google::protobuf::io::ZeroCopyInputStream &input,
               std::size_t size);

    void parse_partial(google::protobuf::Message &message,
                       const std::string &data);
    void parse(google::protobuf::Message &message,
               const std::string &data);

    void parse_partial(google::protobuf::Message &message,
                       const void *data, std::size_t size);
    void parse(google::protobuf::Message &message,
               const void *data, std::size_t size);

    void parse_partial(google::protobuf::Message &message,
                       int file_descriptor);
    void parse(google::protobuf::Message &message,
               int file_descriptor);

    void parse_partial(google::protobuf::Message &message,
                       std::istream &input);
    void parse(google::protobuf::Message &message,
               std::istream &input);

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
