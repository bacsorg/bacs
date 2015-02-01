#pragma once

#include <google/protobuf/message.h>

namespace bunsan{namespace protobuf
{
    void serialize_partial(const google::protobuf::Message &message,
                           google::protobuf::io::CodedOutputStream &output);
    void serialize(const google::protobuf::Message &message,
                   google::protobuf::io::CodedOutputStream &output);

    void serialize_partial(const google::protobuf::Message &message,
                           google::protobuf::io::ZeroCopyOutputStream &output);
    void serialize(const google::protobuf::Message &message,
                   google::protobuf::io::ZeroCopyOutputStream &output);

    void serialize_partial(const google::protobuf::Message &message,
                           void *data, std::size_t size);
    void serialize(const google::protobuf::Message &message,
                   void *data, std::size_t size);

    void serialize_partial(const google::protobuf::Message &message,
                           int file_descriptor);
    void serialize(const google::protobuf::Message &message,
                   int file_descriptor);

    void serialize_partial(const google::protobuf::Message &message,
                           std::ostream &output);
    void serialize(const google::protobuf::Message &message,
                   std::ostream &output);
}}
