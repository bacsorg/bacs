#pragma once

#include <bunsan/stream_enum.hpp>

namespace bunsan {
namespace log {

BUNSAN_STREAM_ENUM_CLASS(severity, (trace, debug, info, warning, error, fatal))

}  // namespace log
}  // namespace bunsan
