#pragma once

#include <bunsan/process/error.hpp>

namespace bunsan {
namespace process {
namespace detail {

struct error : virtual process::error {};

}  // namespace detail
}  // namespace process
}  // namespace bunsan
