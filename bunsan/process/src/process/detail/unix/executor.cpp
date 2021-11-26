#include "executor.hpp"

#include <bunsan/system_error.hpp>

namespace bunsan::process::detail {

executor::executor(const boost::filesystem::path &executable,
                   const std::vector<std::string> &arguments,
                   const std::unordered_map<std::string, std::string> &environment)
    : m_executable(executable.string()),
      m_arguments(arguments),
      m_char_arguments(m_arguments.size() + 1, nullptr),
      m_environment(environment.size()),
      m_char_environment(environment.size() + 1, nullptr) {
  for (std::size_t i = 0; i < m_arguments.size(); ++i)
    m_char_arguments[i] = const_cast<char *>(m_arguments[i].c_str());
  {
    std::size_t i = 0;
    for (const auto &env : environment) {
      BOOST_ASSERT(i < m_environment.size());
      m_environment[i] = env.first + "=" + env.second;
      m_char_environment[i] = const_cast<char *>(m_environment[i].c_str());
      ++i;
    }
    BOOST_ASSERT(i + 1 == m_char_environment.size());
  }
}

void executor::exec() {
  ::execve(m_executable.c_str(), m_char_arguments.data(), m_char_environment.data());
  BOOST_THROW_EXCEPTION(system_error("execve"));
}

}  // namespace bunsan::process::detail
