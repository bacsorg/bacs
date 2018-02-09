#pragma once

#include <bunsan/process/detail/error.hpp>

#include <bunsan/filesystem/error.hpp>

#include <sys/types.h>

namespace bunsan::process::detail {

struct system_error : bunsan::filesystem::system_error, virtual error {
  using bunsan::filesystem::system_error::system_error;

  using pid = boost::error_info<struct tag_pid, pid_t>;
};

}  // namespace bunsan::process::detail
