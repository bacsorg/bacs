#pragma once

#include <bunsan/protobuf/base_io.hpp>
#include <bunsan/protobuf/error.hpp>

#include <google/protobuf/message.h>

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
                    serialize_error() <<
                    serialize_error::message("Uninitialized") <<
                    serialize_error::type_name(message.GetTypeName()) <<
                    serialize_error::initialization_error_string(
                        message.InitializationErrorString()));
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
    };
}}
