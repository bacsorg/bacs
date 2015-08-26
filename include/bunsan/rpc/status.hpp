#pragma once

#include <grpc++/status.h>

#include <boost/exception/to_string.hpp>

#include <string>

namespace grpc {
std::string to_string(const StatusCode status);
std::string to_string(const Status &status);
}  // namespace grpc
