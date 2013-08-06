#pragma once

#include <bacs/archive/problem.hpp>

#include <bunsan/forward_constructor.hpp>

#include <cppcms/filters.h>

namespace cppcms{namespace filters
{
    class base64_binary
    {
    public:
        BUNSAN_FORWARD_EXPLICIT_CONSTRUCTOR(base64_binary, m_binary)

        base64_binary()=default;
        base64_binary(const base64_binary &)=default;
        base64_binary &operator=(const base64_binary &)=default;
        base64_binary(base64_binary &&)=default;
        base64_binary &operator=(base64_binary &&)=default;

        void operator()(std::ostream &out) const;

    private:
        bacs::archive::problem::binary m_binary;
    };

    inline std::ostream &operator<<(std::ostream &out, const base64_binary &obj)
    {
        obj(out);
        return out;
    }
}}
