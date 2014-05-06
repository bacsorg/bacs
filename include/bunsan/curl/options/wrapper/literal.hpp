#pragma once

#include <bunsan/curl/options/traits.hpp>

#include <curl/curl.h>

#include <type_traits>

namespace bunsan{namespace curl{namespace options{namespace wrapper
{
    namespace detail
    {
        template <typename T, T Value, typename CopyPolicy>
        class literal
        {
        public:
            typedef CopyPolicy copy_policy;

            literal()=default;

            inline T data() const { return Value; }
        };
    }

    template <typename T, T Value>
    class enum_literal:
        public detail::literal<
            long,
            static_cast<long>(Value),
            copy_policy::by_curl
        > {};

    template <long Value>
    class long_literal:
        public detail::literal<
            long,
            Value,
            copy_policy::by_curl
        > {};

    template <char ... Chars>
    class string_literal
    {
    public:
        typedef copy_policy::by_curl copy_policy;

        string_literal()=default;

        inline const char *data() const { return m_data; }

    private:
        const char m_data[sizeof...(Chars) + 1] = {
            Chars...,
            '\0'
        };
    };
}}}}
