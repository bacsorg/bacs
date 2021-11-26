#pragma once

#include <boost/filesystem/path.hpp>
#include <boost/noncopyable.hpp>

#include <string>
#include <unordered_map>
#include <vector>

namespace bunsan::process::detail {

class executor : private boost::noncopyable {
 public:
  executor(const boost::filesystem::path &executable,
           const std::vector<std::string> &arguments,
           const std::unordered_map<std::string, std::string> &environment);

  [[noreturn]] void exec();

 private:
  const std::string m_executable;
  std::vector<std::string> m_arguments;
  std::vector<char *> m_char_arguments;
  std::vector<std::string> m_environment;
  std::vector<char *> m_char_environment;
};

}  // namespace bunsan::process::detail
