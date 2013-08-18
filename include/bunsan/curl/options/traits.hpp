#pragma once

namespace bunsan{namespace curl{namespace options
{
    namespace copy_policy
    {
        struct by_wrapper {};
        struct by_curl {};
    }

    template <typename Option>
    struct option_traits
    {
        typedef typename Option::copy_policy copy_policy;
    };
}}}
