#include <bunsan/protobuf/serializer.hpp>

namespace bunsan{namespace protobuf
{
    void serializer::serialize_raw(
        const google::protobuf::Message &message,
        google::protobuf::io::ZeroCopyOutputStream &output)
    {
        if (!message.SerializePartialToZeroCopyStream(&output))
            BOOST_THROW_EXCEPTION(
                serialize_error() <<
                serialize_error::type_name(message.GetTypeName()));
    }
}}
