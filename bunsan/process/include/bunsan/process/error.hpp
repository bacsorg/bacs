#pragma once

#include <bunsan/process/context.hpp>

#include <bunsan/error.hpp>
#include <bunsan/filesystem/error.hpp>

#include <boost/filesystem/path.hpp>

#include <string>
#include <vector>

namespace bunsan {
namespace process {

struct error : virtual bunsan::error {
  using context = boost::error_info<struct tag_context, process::context>;
  using executable =
      boost::error_info<struct tag_executable, boost::filesystem::path>;
  using arguments =
      boost::error_info<struct tag_arguments, std::vector<std::string>>;
  using exit_status = boost::error_info<struct tag_exit_status, int>;
  using output = boost::error_info<struct tag_output, std::string>;
};

struct system_error : bunsan::filesystem::system_error, virtual error {
  using bunsan::filesystem::system_error::system_error;
};

struct nothing_to_execute_error : virtual error {};

struct invalid_executable_error : virtual error {};
struct empty_executable_error : virtual invalid_executable_error {};
struct non_basename_executable_error : virtual invalid_executable_error {};

struct sync_execute_error : virtual error {};
struct sync_execute_with_output_error : virtual sync_execute_error {};

struct check_sync_execute_error : virtual error {};
struct non_zero_exit_status_error : virtual check_sync_execute_error {};

struct context_error : virtual error {};
struct context_build_error : virtual context_error {};

struct find_executable_in_path_error : virtual error {};
struct environment_path_error : virtual find_executable_in_path_error {};
struct no_environment_path_error : virtual environment_path_error {};
struct empty_environment_path_error : virtual environment_path_error {};
struct no_executable_in_path_error : virtual find_executable_in_path_error {};

}  // namespace process
}  // namespace bunsan

namespace boost {
std::string to_string(const bunsan::process::error::context &context);
std::string to_string(const bunsan::process::error::arguments &arguments);
}  // namespace boost
