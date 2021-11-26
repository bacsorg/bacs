#include <bunsan/process/path.hpp>

#include <bunsan/process/error.hpp>

#include <boost/process/search_path.hpp>

namespace bunsan::process {

boost::filesystem::path find_executable_in_path(
    const boost::filesystem::path &executable) {
  try {
    if (executable != executable.filename()) {
      BOOST_THROW_EXCEPTION(
          non_basename_executable_error()
          << non_basename_executable_error::executable(executable));
    }
    const boost::filesystem::path path =
        boost::process::search_path(executable);
    if (path.empty()) {
      BOOST_THROW_EXCEPTION(
          no_executable_in_path_error()
          << no_executable_in_path_error::executable(executable));
    }
    return path;
  } catch (find_executable_in_path_error &) {
    throw;
  } catch (std::exception &) {
    BOOST_THROW_EXCEPTION(
        find_executable_in_path_error()
        << find_executable_in_path_error::executable(executable)
        << enable_nested_current());
  }
}

}  // namespace bunsan::process
