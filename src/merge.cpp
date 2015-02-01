#include <bunsan/protobuf/merge.hpp>

#include "helper.hpp"

#include <bunsan/protobuf/error.hpp>

namespace bunsan{namespace protobuf
{
    void merge(google::protobuf::Message &message,
               google::protobuf::io::CodedInputStream &input)
    {
        merge_partial(message, input);
        BUNSAN_PROTOBUF_VERIFY_INITIALIZED(message, parse_error);
    }

    void merge_partial(google::protobuf::Message &message,
                       google::protobuf::io::CodedInputStream &input)
    {
        BUNSAN_PROTOBUF_OPERATION(
            message,
            MergePartialFromCodedStream(&input),
            parse_error
        );
    }
}}
