#include <bunsan/protobuf/serialize.hpp>

#include "helper.hpp"

#include <bunsan/protobuf/error.hpp>

namespace bunsan{namespace protobuf
{
    void serialize(const google::protobuf::Message &message,
                   google::protobuf::io::CodedOutputStream &output)
    {
        BUNSAN_PROTOBUF_VERIFY_INITIALIZED(message, serialize_error);
        BUNSAN_PROTOBUF_OPERATION(
            message,
            SerializeToCodedStream(&output),
            serialize_error
        );
    }

    void serialize_partial(const google::protobuf::Message &message,
                           google::protobuf::io::CodedOutputStream &output)
    {
        BUNSAN_PROTOBUF_OPERATION(
            message,
            SerializePartialToCodedStream(&output),
            serialize_error
        );
    }

    void serialize(const google::protobuf::Message &message,
                   google::protobuf::io::ZeroCopyOutputStream &output)
    {
        BUNSAN_PROTOBUF_VERIFY_INITIALIZED(message, serialize_error);
        BUNSAN_PROTOBUF_OPERATION(
            message,
            SerializeToZeroCopyStream(&output),
            serialize_error
        );
    }

    void serialize_partial(const google::protobuf::Message &message,
                           google::protobuf::io::ZeroCopyOutputStream &output)
    {
        BUNSAN_PROTOBUF_OPERATION(
            message,
            SerializePartialToZeroCopyStream(&output),
            serialize_error
        );
    }

    void serialize(const google::protobuf::Message &message,
                   void *data, std::size_t size)
    {
        BUNSAN_PROTOBUF_INT_CAST(int_size, size, serialize_error);
        BUNSAN_PROTOBUF_VERIFY_INITIALIZED(message, serialize_error);
        BUNSAN_PROTOBUF_OPERATION(
            message,
            SerializeToArray(data, int_size),
            serialize_error
        );
    }

    void serialize_partial(const google::protobuf::Message &message,
                           void *data, std::size_t size)
    {
        BUNSAN_PROTOBUF_INT_CAST(int_size, size, serialize_error);
        BUNSAN_PROTOBUF_OPERATION(
            message,
            SerializePartialToArray(data, int_size),
            serialize_error
        );
    }

    void serialize(const google::protobuf::Message &message,
                   int file_descriptor)
    {
        BUNSAN_PROTOBUF_VERIFY_INITIALIZED(message, serialize_error);
        BUNSAN_PROTOBUF_OPERATION(
            message,
            SerializeToFileDescriptor(file_descriptor),
            serialize_error
        );
    }

    void serialize_partial(const google::protobuf::Message &message,
                           int file_descriptor)
    {
        BUNSAN_PROTOBUF_OPERATION(
            message,
            SerializePartialToFileDescriptor(file_descriptor),
            serialize_error
        );
    }

    void serialize(const google::protobuf::Message &message,
                   std::ostream &output)
    {
        BUNSAN_PROTOBUF_VERIFY_INITIALIZED(message, serialize_error);
        BUNSAN_PROTOBUF_OPERATION(
            message,
            SerializeToOstream(&output),
            serialize_error
        );
    }

    void serialize_partial(const google::protobuf::Message &message,
                           std::ostream &output)
    {
        BUNSAN_PROTOBUF_OPERATION(
            message,
            SerializePartialToOstream(&output),
            serialize_error
        );
    }
}}
