#include <bunsan/process/path.hpp>

#include <bunsan/process/error.hpp>

#include <boost/algorithm/string/classification.hpp>
#include <boost/algorithm/string/split.hpp>
#include <boost/filesystem/operations.hpp>

#include <cstdlib>

namespace bunsan {
namespace process {

boost::filesystem::path find_executable_in_path(
    const boost::filesystem::path &executable) {
  try {
    if (executable != executable.filename())
      BOOST_THROW_EXCEPTION(
          non_basename_executable_error()
          << non_basename_executable_error::executable(executable));

    const char *const envpath = ::getenv("PATH");
    if (!envpath) BOOST_THROW_EXCEPTION(no_environment_path_error());

    /*const*/ std::string path(envpath);
    if (path.empty()) BOOST_THROW_EXCEPTION(empty_environment_path_error());

#if defined(__CYGWIN__)
    if (!::cygwin_posix_path_list_p(path.c_str())) {
      const int size = ::cygwin_win32_to_posix_path_list_buf_size(path.c_str());
      std::vector<char> cygpath(size);
      ::cygwin_win32_to_posix_path_list(path.c_str(), cygpath.data());
      path = cygpath.data();
    }
#endif

    std::vector<std::string> path_dirs;
    boost::algorithm::split(path_dirs, path, boost::algorithm::is_any_of(":"),
                            boost::algorithm::token_compress_on);

    for (const boost::filesystem::path dir : path_dirs) {
      try {
        const boost::filesystem::path abs_path =
            boost::filesystem::absolute(dir / executable);
        if (boost::filesystem::exists(abs_path)) return abs_path;
      } catch (boost::filesystem::filesystem_error &) {
        // skip unaccessible path
        continue;
      }
    }

    BOOST_THROW_EXCEPTION(
        no_executable_in_path_error()
        << no_executable_in_path_error::executable(executable));
  } catch (find_executable_in_path_error &) {
    throw;
  } catch (std::exception &) {
    BOOST_THROW_EXCEPTION(
        find_executable_in_path_error()
        << find_executable_in_path_error::executable(executable)
        << enable_nested_current());
  }
}

}  // namespace process
}  // namespace bunsan
