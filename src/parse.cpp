#include <bunsan/protobuf/parse.hpp>

#include "helper.hpp"

#include <bunsan/protobuf/error.hpp>

namespace bunsan{namespace protobuf
{
    void parse(google::protobuf::Message &message,
               google::protobuf::io::CodedInputStream &input)
    {
        parse_partial(message, input);
        BUNSAN_PROTOBUF_VERIFY_INITIALIZED(message, parse_error);
    }

    void parse_partial(google::protobuf::Message &message,
                       google::protobuf::io::CodedInputStream &input)
    {
        BUNSAN_PROTOBUF_OPERATION(
            message,
            ParsePartialFromCodedStream(&input),
            parse_error
        );
    }

    void parse(google::protobuf::Message &message,
               google::protobuf::io::ZeroCopyInputStream &input)
    {
        parse_partial(message, input);
        BUNSAN_PROTOBUF_VERIFY_INITIALIZED(message, parse_error);
    }

    void parse_partial(google::protobuf::Message &message,
                       google::protobuf::io::ZeroCopyInputStream &input)
    {
        BUNSAN_PROTOBUF_OPERATION(
            message,
            ParsePartialFromZeroCopyStream(&input),
            parse_error
        );
    }

    void parse(google::protobuf::Message &message,
               google::protobuf::io::ZeroCopyInputStream &input,
               std::size_t size)
    {
        parse_partial(message, input, size);
        BUNSAN_PROTOBUF_VERIFY_INITIALIZED(message, parse_error);
    }

    void parse_partial(google::protobuf::Message &message,
                       google::protobuf::io::ZeroCopyInputStream &input,
                       std::size_t size)
    {
        BUNSAN_PROTOBUF_INT_CAST(int_size, size, parse_error);
        BUNSAN_PROTOBUF_OPERATION(
            message,
            ParsePartialFromBoundedZeroCopyStream(&input, int_size),
            parse_error
        );
    }

    void parse(google::protobuf::Message &message,
               const std::string &data)
    {
        parse_partial(message, data);
        BUNSAN_PROTOBUF_VERIFY_INITIALIZED(message, parse_error);
    }

    void parse_partial(google::protobuf::Message &message,
               const std::string &data)
    {
        BUNSAN_PROTOBUF_OPERATION(
            message,
            ParsePartialFromString(data),
            parse_error
        );
    }

    void parse(google::protobuf::Message &message,
               const void *data, std::size_t size)
    {
        parse_partial(message, data, size);
        BUNSAN_PROTOBUF_VERIFY_INITIALIZED(message, parse_error);
    }

    void parse_partial(google::protobuf::Message &message,
                       const void *data, std::size_t size)
    {
        BUNSAN_PROTOBUF_INT_CAST(int_size, size, parse_error);
        BUNSAN_PROTOBUF_OPERATION(
            message,
            ParsePartialFromArray(data, int_size),
            parse_error
        );
    }

    void parse(google::protobuf::Message &message,
               int file_descriptor)
    {
        parse_partial(message, file_descriptor);
        BUNSAN_PROTOBUF_VERIFY_INITIALIZED(message, parse_error);
    }

    void parse_partial(google::protobuf::Message &message,
                       int file_descriptor)
    {
        BUNSAN_PROTOBUF_OPERATION(
            message,
            ParsePartialFromFileDescriptor(file_descriptor),
            parse_error
        );
    }

    void parse(google::protobuf::Message &message,
               std::istream &input)
    {
        parse_partial(message, input);
        BUNSAN_PROTOBUF_VERIFY_INITIALIZED(message, parse_error);
    }

    void parse_partial(google::protobuf::Message &message,
                       std::istream &input)
    {
        BUNSAN_PROTOBUF_OPERATION(
            message,
            ParsePartialFromIstream(&input),
            parse_error
        );
    }
}}
