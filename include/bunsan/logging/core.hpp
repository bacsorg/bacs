#pragma once

#include <bunsan/logging/severity.hpp>

namespace bunsan{namespace logging
{
    bool set_enabled(bool enabled);

    inline bool enable() { return set_enabled(true); }
    inline bool disable() { return set_enabled(false); }

    void remove_default_sink();

    void set_minimum_severity(const severity minimum_severity);
}}
