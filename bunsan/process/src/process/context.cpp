#include <bunsan/process/context.hpp>

#include <bunsan/process/error.hpp>

#include <boost/filesystem/operations.hpp>

#include <sstream>

namespace boost {
#define BUNSAN_PROCESS_CONTEXT_PRINT(FIELD) \
  if (first_field)                          \
    first_field = false;                    \
  else                                      \
    sout << ", ";                           \
  sout << #FIELD << " = ";

std::string to_string(const bunsan::process::context &context) {
  std::ostringstream sout;
  sout << "{ ";
  bool first_field = true;

  if (context.m_executable) {
    BUNSAN_PROCESS_CONTEXT_PRINT(executable);
    sout << context.m_executable;
  }

  // arguments
  {
    BUNSAN_PROCESS_CONTEXT_PRINT(arguments);
    sout << '[';
    for (std::size_t i = 0; i < context.m_arguments.size(); ++i) {
      if (i)
        sout << ", ";
      else
        sout << ' ';
      sout << boost::io::quoted(context.m_arguments[i]);
    }
    sout << " ]";
  }

  if (!first_field) sout << ' ';
  sout << '}';

  return sout.str();
}
}  // namespace boost

namespace bunsan {
namespace process {

void bunsan::process::context::build_() {
  try {
    if (m_arguments.empty() && !m_executable)
      BOOST_THROW_EXCEPTION(
          nothing_to_execute_error() << nothing_to_execute_error::message(
              "arguments are empty and executable is not set"));
    else if (m_arguments.empty() && m_executable)
      m_arguments.push_back(m_executable->string());
    else if (!m_arguments.empty() && !m_executable)
      m_executable = m_arguments[0];

    if (!m_current_path) m_current_path = boost::filesystem::current_path();

    if (!m_use_path) m_use_path = m_executable->filename() == *m_executable;
    if (*m_use_path && m_executable->is_absolute()) m_use_path = false;
    if (*m_use_path) {
      if (m_executable->empty())
        BOOST_THROW_EXCEPTION(empty_executable_error());
      if (*m_executable != m_executable->filename())
        BOOST_THROW_EXCEPTION(
            non_basename_executable_error()
            << non_basename_executable_error::executable(*m_executable));
    } else {
      // TODO if we do not use path,
      // is it preferred to have absolute path to the executable file
      // or should we raise an exception?
      m_executable = boost::filesystem::absolute(*m_executable);
    }
  } catch (std::exception &) {
    BOOST_THROW_EXCEPTION(context_build_error() << enable_nested_current());
  }
}

}  // namespace process
}  // namespace bunsan
