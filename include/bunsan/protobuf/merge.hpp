#pragma once

#include <google/protobuf/message.h>

namespace bunsan{namespace protobuf
{
    void merge_partial(google::protobuf::Message &message,
                       google::protobuf::io::CodedInputStream &input);
    void merge(google::protobuf::Message &message,
               google::protobuf::io::CodedInputStream &input);
}}
