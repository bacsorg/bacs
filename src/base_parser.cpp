#include <bunsan/protobuf/base_parser.hpp>

#include <bunsan/filesystem/fstream.hpp>

#include <google/protobuf/io/zero_copy_stream_impl.h>
#include <google/protobuf/io/zero_copy_stream_impl_lite.h>

#include <boost/numeric/conversion/cast.hpp>

namespace bunsan{namespace protobuf
{
    void base_parser::merge_raw(
        google::protobuf::Message &message,
        google::protobuf::io::ZeroCopyInputStream &input,
        std::size_t size)
    {
        google::protobuf::io::LimitingInputStream stream(
            &input,
            boost::numeric_cast<std::int64_t>(size)
        );
        merge_raw(message, stream);
    }

    void base_parser::merge_raw(
        google::protobuf::Message &message,
        const void *data,
        std::size_t size)
    {
        google::protobuf::io::ArrayInputStream stream(
            data,
            boost::numeric_cast<int>(size)
        );
        merge_raw(message, stream);
    }

    void base_parser::merge_raw(
        google::protobuf::Message &message,
        const std::string &input)
    {
        merge_raw(message, input.data(), input.size());
    }

    void base_parser::merge_raw(
        google::protobuf::Message &message,
        std::istream &input)
    {
        google::protobuf::io::IstreamInputStream stream(&input);
        merge_raw(message, stream);
    }

    void base_parser::merge_raw(
        google::protobuf::Message &message,
        const boost::filesystem::path &path)
    {
        bunsan::filesystem::ifstream fin(path);
        BUNSAN_FILESYSTEM_FSTREAM_WRAP_BEGIN(fin)
        {
            merge_raw(message, fin);
        }
        BUNSAN_FILESYSTEM_FSTREAM_WRAP_END(fin)
        fin.close();
    }
}}
