#pragma once

#include <bunsan/log/severity.hpp>

namespace bunsan {
namespace log {

bool set_enabled(bool enabled);

inline bool enable() { return set_enabled(true); }
inline bool disable() { return set_enabled(false); }

void remove_default_sink();

void set_minimum_severity(const severity minimum_severity);

}  // namespace log
}  // namespace bunsan
