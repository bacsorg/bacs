#include <bunsan/protobuf/base_serializer.hpp>

#include <google/protobuf/io/zero_copy_stream_impl.h>
#include <google/protobuf/io/zero_copy_stream_impl_lite.h>

#include <boost/numeric/conversion/cast.hpp>

namespace bunsan{namespace protobuf
{
    void base_serializer::serialize_raw(
        const google::protobuf::Message &message,
        std::ostream &output)
    {
        google::protobuf::io::OstreamOutputStream stream(&output);
        serialize_raw(message, stream);
    }

    void base_serializer::serialize_raw(
        const google::protobuf::Message &message,
        std::string &output,
        string_mode mode)
    {
        if (mode == string_mode::replace)
            output.clear();
        google::protobuf::io::StringOutputStream stream(&output);
        serialize_raw(message, stream);
    }
}}
