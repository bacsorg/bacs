#pragma once

#include <bunsan/process/error.hpp>
#include <bunsan/process/file/handle.hpp>

#if defined(BOOST_POSIX_API)
#include <sys/types.h>
#endif

namespace bunsan {
namespace process {
namespace file {

struct error : virtual process::error {
  using handle =
      boost::error_info<struct tag_handle, file::handle::implementation>;
  using new_handle =
      boost::error_info<struct tag_new_handle, file::handle::implementation>;
#if defined(BOOST_POSIX_API)
  using flags = boost::error_info<struct tag_flags, int>;
  using mode = boost::error_info<struct tag_mode, mode_t>;
#endif
};

struct system_error : process::system_error, virtual error {
  using process::system_error::system_error;
};

struct handle_error : virtual error {};
struct handle_is_closed_error : virtual error {};

}  // namespace file
}  // namespace process
}  // namespace bunsan
