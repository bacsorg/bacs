#include <bunsan/process/detail/execute.hpp>

#include <bunsan/process/error.hpp>

#include <boost/algorithm/string/join.hpp>
#include <boost/nowide/convert.hpp>
#include <boost/range/adaptor/transformed.hpp>

#include <windows.h>

using boost::nowide::widen;

namespace bunsan {
namespace process {
namespace detail {

namespace {
std::string win32_escape_argument(const std::string &arg) {
  const bool has_spaces = arg.find(' ') != std::string::npos;
  std::string ret;
  if (has_spaces) ret.push_back('"');
  for (const char c : arg) {
    if (c == '"') ret.push_back('\\');
    ret.push_back(c);
  }
  if (has_spaces) ret.push_back('"');
  return ret;
}

std::string win32_escape_arguments(const std::vector<std::string> &args) {
  return boost::algorithm::join(
      args | boost::adaptors::transformed(win32_escape_argument), " ");
}
}  // namespace

int sync_execute(context ctx) {
  STARTUPINFOW si;
  ::ZeroMemory(&si, sizeof(si));
  PROCESS_INFORMATION pi;
  ::ZeroMemory(&pi, sizeof(pi));
  si.dwFlags |= STARTF_USESTDHANDLES;  // use std{in,out,err} as files
  si.hStdInput = *ctx.stdin_file;
  si.hStdOutput = *ctx.stdout_file;
  si.hStdError = *ctx.stderr_file;
  const std::string cmdline = win32_escape_arguments(ctx.arguments);
  const std::wstring cmdlinew = widen(cmdline);
  std::vector<wchar_t> cmdlinewm(cmdlinew.begin(), cmdlinew.end());
  cmdlinewm.push_back(L'\0');
  if (!::CreateProcessW(ctx.executable.c_str(), cmdlinewm.data(), nullptr,
                        nullptr, true, 0, nullptr, ctx.current_path.c_str(),
                        &si, &pi))
    BOOST_THROW_EXCEPTION(system_error("CreateProcess")
                          << system_error::path(ctx.executable));
  switch (::WaitForSingleObject(pi.hProcess, INFINITE)) {
    case WAIT_OBJECT_0:
      // everything is OK, continue below
      break;
    case WAIT_FAILED:
      BOOST_THROW_EXCEPTION(system_error("WaitForSingleObject")
                            << system_error::path(ctx.executable));
    default:
      BOOST_ASSERT(false);
  }
  DWORD exit_code;
  if (!::GetExitCodeProcess(pi.hProcess, &exit_code))
    BOOST_THROW_EXCEPTION(system_error("GetExitCodeProcess")
                          << system_error::path(ctx.executable));
  return exit_code;
}

}  // namespace detail
}  // namespace process
}  // namespace bunsan
