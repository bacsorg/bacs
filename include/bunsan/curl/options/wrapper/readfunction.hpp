#pragma once

#include <bunsan/curl/options/traits.hpp>
#include <bunsan/curl/options/wrapper/basic_function.hpp>

#include <curl/curl.h>

#include <functional>
#include <utility>

namespace bunsan{namespace curl{namespace options{namespace wrapper
{
    struct readfunction_traits
    {
        typedef basic_function<readfunction_traits> wrapper;

        typedef std::function<
            std::size_t (char *ptr, size_t size)
        > function_type;

        static inline std::size_t static_call(
            void *ptr, size_t size, size_t nmemb, void *userdata)
        {
            const auto this_ = static_cast<const wrapper *>(userdata);
            return this_->call(static_cast<char *>(ptr), size * nmemb);
        }
    };

    typedef readfunction_traits::wrapper readfunction;
}}}}
