#include "executor.hpp"

#include <bunsan/system_error.hpp>

namespace bunsan {
namespace process {
namespace detail {

executor::executor(const boost::filesystem::path &executable,
                   const std::vector<std::string> &arguments)
    : m_executable(executable.string()),
      m_arguments(arguments),
      m_char_arguments(m_arguments.size() + 1, nullptr) {
  for (std::size_t i = 0; i < m_arguments.size(); ++i)
    m_char_arguments[i] = const_cast<char *>(m_arguments[i].c_str());
}

void executor::exec() {
  ::execv(m_executable.c_str(), m_char_arguments.data());
  BOOST_THROW_EXCEPTION(system_error("execv"));
}

}  // namespace detail
}  // namespace process
}  // namespace bunsan
