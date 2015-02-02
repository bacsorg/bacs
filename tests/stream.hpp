#pragma once

#include IO_HEADER

#include <google/protobuf/io/coded_stream.h>
#include <google/protobuf/io/zero_copy_stream_impl_lite.h>

#include <boost/mpl/list.hpp>
#include <boost/numeric/conversion/cast.hpp>

#include <sstream>
#include <tuple>

#define BUNSAN_PROTOBUF_INPUT(...) \
    template <typename Message> \
    void merge_partial(Message &message) \
    { \
        bunsan::protobuf::IO::merge_partial(message, __VA_ARGS__); \
    } \
    template <typename Message> \
    void merge(Message &message) \
    { \
        bunsan::protobuf::IO::merge(message, __VA_ARGS__); \
    } \
    template <typename Message> \
    void parse_partial(Message &message) \
    { \
        bunsan::protobuf::IO::parse_partial(message, __VA_ARGS__); \
    } \
    template <typename Message> \
    void parse(Message &message) \
    { \
        bunsan::protobuf::IO::parse(message, __VA_ARGS__); \
    } \
    template <typename Message> \
    Message parse_partial_make() \
    { \
        return bunsan::protobuf::IO::parse_partial_make<Message>(__VA_ARGS__); \
    } \
    template <typename Message> \
    Message parse_make() \
    { \
        return bunsan::protobuf::IO::parse_make<Message>(__VA_ARGS__); \
    }

#define BUNSAN_PROTOBUF_OUTPUT(...) \
    template <typename Message> \
    void serialize_partial(const Message &message) \
    { \
        bunsan::protobuf::IO::serialize_partial(message, __VA_ARGS__); \
    } \
    template <typename Message> \
    void serialize(const Message &message) \
    { \
        bunsan::protobuf::IO::serialize(message, __VA_ARGS__); \
    }

template <typename Stream>
struct stream;

template <>
struct stream<google::protobuf::io::ZeroCopyInputStream>
{
    explicit stream(const std::string &data):
        input_stream(data.data(), boost::numeric_cast<int>(data.size())) {}

    BUNSAN_PROTOBUF_INPUT(input_stream)

    google::protobuf::io::ArrayInputStream input_stream;
};

template <>
struct stream<google::protobuf::io::ZeroCopyOutputStream>
{
    stream(): output_stream(&str) {}

    std::string data() const { return str; }

    BUNSAN_PROTOBUF_OUTPUT(output_stream)

    std::string str;
    google::protobuf::io::StringOutputStream output_stream;
};

template <>
struct stream<
    boost::mpl::list<google::protobuf::io::ZeroCopyInputStream, std::size_t>
>
{
    explicit stream(const std::string &data):
        input_stream(data.data(),
                     boost::numeric_cast<int>(data.size())),
        size(data.size()) {}

    BUNSAN_PROTOBUF_INPUT(input_stream, size)

    google::protobuf::io::ArrayInputStream input_stream;
    std::size_t size;
};

template <>
struct stream<std::string>
{
    stream()=default;
    explicit stream(const std::string &data): str(data) {}

    std::string data() const { return str; }

    BUNSAN_PROTOBUF_INPUT(str)

    std::string str;
};

template <>
struct stream<boost::mpl::list<std::string, struct append_tag>>:
    stream<std::string>
{
    using stream<std::string>::stream;

    BUNSAN_PROTOBUF_OUTPUT(
        str,
        bunsan::protobuf::base_serializer::string_mode::append
    )
};

template <>
struct stream<boost::mpl::list<std::string, struct replace_tag>>:
    stream<std::string>
{
    using stream<std::string>::stream;

    BUNSAN_PROTOBUF_OUTPUT(
        str,
        bunsan::protobuf::base_serializer::string_mode::replace
    )
};

template <>
struct stream<boost::mpl::list<const void *, std::size_t>>
{
    explicit stream(const std::string &data):
        ptr(data.data()),
        size(data.size()) {}

    BUNSAN_PROTOBUF_INPUT(ptr, size)

    const void *ptr;
    const std::size_t size;
};

template <>
struct stream<std::istream>
{
    explicit stream(const std::string &data): ss(data) {}

    BUNSAN_PROTOBUF_INPUT(ss)

    std::istringstream ss;
};

template <>
struct stream<std::ostream>
{
    stream()=default;

    std::string data() const { return ss.str(); }

    BUNSAN_PROTOBUF_OUTPUT(ss)

    std::ostringstream ss;
};
