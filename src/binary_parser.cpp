#include <bunsan/protobuf/binary_parser.hpp>

#include <bunsan/protobuf/error.hpp>

#include <google/protobuf/io/coded_stream.h>

namespace bunsan{namespace protobuf
{
    void binary_parser::merge_raw(
        google::protobuf::Message &message,
        google::protobuf::io::ZeroCopyInputStream &input)
    {
        google::protobuf::io::CodedInputStream stream(&input);
        if (!message.MergePartialFromCodedStream(&stream))
            BOOST_THROW_EXCEPTION(
                parse_error() <<
                parse_error::type_name(message.GetTypeName()));
    }
}}
