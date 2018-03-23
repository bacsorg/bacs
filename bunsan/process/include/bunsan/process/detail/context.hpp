#pragma once

#include <bunsan/process/file/handle.hpp>

#include <boost/filesystem/path.hpp>
#include <boost/optional.hpp>

#include <string>
#include <unordered_map>
#include <vector>

namespace bunsan::process::detail {

struct context {
  boost::filesystem::path executable;
  boost::filesystem::path current_path;
  std::vector<std::string> arguments;
  std::unordered_map<std::string, std::string> environment;
  file::handle stdin_file, stdout_file, stderr_file;
};

}  // namespace bunsan::process::detail
