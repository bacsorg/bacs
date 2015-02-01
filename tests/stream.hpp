#pragma once

#include <bunsan/protobuf/io.hpp>

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
        bunsan::protobuf::merge_partial(message, __VA_ARGS__); \
    } \
    template <typename Message> \
    void merge(Message &message) \
    { \
        bunsan::protobuf::merge(message, __VA_ARGS__); \
    } \
    template <typename Message> \
    void parse_partial(Message &message) \
    { \
        bunsan::protobuf::parse_partial(message, __VA_ARGS__); \
    } \
    template <typename Message> \
    void parse(Message &message) \
    { \
        bunsan::protobuf::parse(message, __VA_ARGS__); \
    } \
    template <typename Message> \
    Message parse_partial_make() \
    { \
        return bunsan::protobuf::parse_partial_make<Message>(__VA_ARGS__); \
    } \
    template <typename Message> \
    Message parse_make() \
    { \
        return bunsan::protobuf::parse_make<Message>(__VA_ARGS__); \
    }

#define BUNSAN_PROTOBUF_OUTPUT(...) \
    template <typename Message> \
    void serialize_partial(const Message &message) \
    { \
        bunsan::protobuf::serialize_partial(message, __VA_ARGS__); \
    } \
    template <typename Message> \
    void serialize(const Message &message) \
    { \
        bunsan::protobuf::serialize(message, __VA_ARGS__); \
    }


template <typename Stream>
struct stream;

template <>
struct stream<google::protobuf::io::CodedInputStream>
{
    explicit stream(const std::string &data):
        coded_stream(reinterpret_cast<const unsigned char *>(data.data()),
                     boost::numeric_cast<int>(data.size())) {}

    BUNSAN_PROTOBUF_INPUT(coded_stream)

    google::protobuf::io::CodedInputStream coded_stream;
};

template <>
struct stream<google::protobuf::io::CodedOutputStream>
{
    stream():
        output_stream(&str),
        coded_stream(
            new google::protobuf::io::CodedOutputStream(&output_stream)
        ) {}

    std::string data()
    {
        coded_stream.reset(); // flush
        return str;
    }

    BUNSAN_PROTOBUF_OUTPUT(*coded_stream)

    std::string str;
    google::protobuf::io::StringOutputStream output_stream;
    std::unique_ptr<google::protobuf::io::CodedOutputStream> coded_stream;
};

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
    BUNSAN_PROTOBUF_OUTPUT(str)

    std::string str;
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
struct stream<boost::mpl::list<void *, std::size_t>>
{
    // Size should be enough for any small testing buffer
    stream(): buffer(1024 * 1024) {}

    std::string data() const
    {
        std::size_t last_zero = buffer.size() - 1;
        while (last_zero < buffer.size() && buffer[last_zero] == '\0')
            --last_zero;
        ++last_zero;
        return std::string(&buffer[0], last_zero);
    }

    BUNSAN_PROTOBUF_OUTPUT(buffer.data(), buffer.size())

    std::vector<char> buffer;
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
