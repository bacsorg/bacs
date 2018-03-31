#include <bunsan/utility/system_resolver.hpp>

#include <bunsan/process/path.hpp>

namespace bunsan::utility {

boost::filesystem::path system_resolver::find_executable(
    const boost::filesystem::path &exe) {
  return bunsan::process::find_executable_in_path(exe);
}

boost::filesystem::path system_resolver::find_library(
    const boost::filesystem::path &lib) {
#warning unimplemented
  return lib;
}

}  // namespace bunsan::utility
