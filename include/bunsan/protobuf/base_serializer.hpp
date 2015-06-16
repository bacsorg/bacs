#pragma once

#include <bunsan/protobuf/base_io.hpp>
#include <bunsan/protobuf/error.hpp>
#include <bunsan/protobuf/initialization.hpp>

#include <google/protobuf/message.h>

#include <boost/filesystem/path.hpp>
#include <boost/noncopyable.hpp>

#include <utility>

namespace bunsan{namespace protobuf
{
    class base_serializer: public base_io
    {
    public:
        enum class string_mode
        {
            append,
            replace
        };

        template <typename ... Args>
        void serialize(const google::protobuf::Message &message, Args &&...args)
        {
            if (!allow_partial() && !message.IsInitialized())
            {
                BOOST_THROW_EXCEPTION(
                    fill_initialization_error(
                        serialize_initialization_error(),
                        message
                    )
                );
            }
            try
            {
                serialize_raw(message, std::forward<Args>(args)...);
            }
            catch (std::exception &)
            {
                BOOST_THROW_EXCEPTION(
                    serialize_error() <<
                    serialize_error::type_name(message.GetTypeName()) <<
                    enable_nested_current());
            }
        }

        void to_string(const google::protobuf::Message &message,
                       std::string &output)
        {
            serialize(message, output, string_mode::replace);
        }

        std::string to_string(const google::protobuf::Message &message)
        {
            std::string string;
            to_string(message, string);
            return string;
        }

        void append(const google::protobuf::Message &message,
                    std::string &output)
        {
            serialize(message, output, string_mode::append);
        }

    protected:
        virtual void serialize_raw(
            const google::protobuf::Message &message,
            google::protobuf::io::ZeroCopyOutputStream &output)=0;

        virtual void serialize_raw(
            const google::protobuf::Message &message,
            std::ostream &output);

        virtual void serialize_raw(
            const google::protobuf::Message &message,
            std::string &output,
            string_mode mode);
        virtual void serialize_raw(
            const google::protobuf::Message &message,
            const boost::filesystem::path &path);
    };
}}
