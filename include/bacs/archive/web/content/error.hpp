#pragma once

#include <bacs/archive/web/content/base.hpp>

namespace bacs{namespace archive{namespace web{namespace content
{
    struct error: base
    {
        std::string brief;
        std::string message;
        bool raw = false; // Should it be rendered as raw html?
    };
}}}}
