#pragma once

#include <boost/numeric/conversion/cast.hpp>

#define BUNSAN_PROTOBUF_INT_CAST(int_var, other_var, Error) \
    int int_var; \
    do { \
        try \
        { \
            int_var = boost::numeric_cast<int>(other_var); \
        } \
        catch (std::exception &) \
        { \
            BOOST_THROW_EXCEPTION( \
                Error() << \
                bunsan::enable_nested_current()); \
        } \
    } while (false)

#define BUNSAN_PROTOBUF_OPERATION(message, statement, Error) \
    do { \
        if (!message.statement) \
        { \
            BOOST_THROW_EXCEPTION( \
                Error() << \
                Error::message(#statement) << \
                Error::type_name(message.GetTypeName())); \
        } \
    } while (false)

#define BUNSAN_PROTOBUF_VERIFY_INITIALIZED(message, Error) \
    do { \
        if (!message.IsInitialized()) \
        { \
            BOOST_THROW_EXCEPTION( \
                Error() << \
                Error::message("Uninitialized") << \
                Error::type_name(message.GetTypeName()) << \
                Error::initialization_error_string( \
                    message.InitializationErrorString())); \
        } \
    } while (false)
