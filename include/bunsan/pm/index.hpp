#pragma once

namespace bunsan{namespace pm{namespace index
{
    constexpr const char *package = "package";
    namespace source
    {
        namespace import
        {
            constexpr const char *package = "source.import.package";
            constexpr const char *source = "source.import.source";
        }
        constexpr const char *self = "source.self";
    }
}}}
