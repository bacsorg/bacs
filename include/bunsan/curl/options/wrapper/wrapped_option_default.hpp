#pragma once

#include <bunsan/curl/options/traits.hpp>

#include <curl/curl.h>

#include <boost/optional.hpp>

namespace bunsan{namespace curl{namespace options{namespace wrapper
{
    template <typename Wrapper, decltype(static_cast<Wrapper *>(nullptr)->data()) Default>
    class wrapped_option_default
    {
    public:
        typedef typename Wrapper::copy_policy copy_policy;

    public:
        wrapped_option_default()=default;

        template <typename Arg, typename ... Args>
        explicit wrapped_option_default(Arg &&arg, Args &&...args):
            m_wrapper(std::forward<Arg>(arg), std::forward<Args>(args)...) {}

        inline decltype(static_cast<Wrapper *>(nullptr)->data()) data() const
        {
            if (m_wrapper)
                return m_wrapper->data();
            else
                return Default;
        }

    private:
        boost::optional<Wrapper> m_wrapper;
    };
}}}}

#define BUNSAN_CURL_OPTION_DEFAULT_WRAPPED(NAME, CODE, ...) \
    typedef ::bunsan::curl::options::wrapper::wrapped_option< \
        CODE, ::bunsan::curl::options::wrapper::wrapped_option_default< \
            ::bunsan::curl::options::wrapper::__VA_ARGS__ \
        > \
    > NAME;
