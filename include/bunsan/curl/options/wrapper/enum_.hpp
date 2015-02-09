#pragma once

#include <bunsan/curl/options/traits.hpp>

#include <curl/curl.h>

#include <boost/cast.hpp>

#include <type_traits>

namespace bunsan{namespace curl{namespace options{namespace wrapper
{
    template <typename Enum>
    class enum_
    {
    public:
        using copy_policy = copy_policy::by_curl;

        explicit enum_(const Enum data): m_data(data) {}

        inline long data() const
        {
            return boost::numeric_cast<long>(
                static_cast<typename std::underlying_type<Enum>::type>(m_data)
            );
        }

    private:
        Enum m_data;
    };

    template <typename Enum, Enum Default>
    class enum_optional: public enum_<Enum>
    {
    public:
        using enum_<Enum>::enum_;

        enum_optional(): enum_<Enum>(Default) {}
    };
}}}}
