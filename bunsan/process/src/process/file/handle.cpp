#include <bunsan/process/file/handle.hpp>

#include <bunsan/process/file/error.hpp>
#include <bunsan/system_error.hpp>

#include <boost/assert.hpp>
#include <boost/scope_exit.hpp>

#if defined(BOOST_POSIX_API)
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#elif defined(BOOST_WINDOWS_API)
#include <windows.h>
#endif

namespace bunsan {
namespace process {
namespace file {

handle::~handle() {
  if (m_close) close_no_except();
}

handle::handle(const implementation fd) : m_fd(fd) {}

handle::handle(const implementation fd, const bool close)
    : m_fd(fd), m_close(close) {}

handle::handle(handle &&o) {
  swap(o);
  o.reset();
}

handle &handle::operator=(handle &&o) {
  swap(o);
  o.reset();
  return *this;
}

void handle::reset() { handle().swap(*this); }

void handle::reset(const implementation fd) { handle(fd).swap(*this); }

void handle::close() {
  if (m_fd) {
    BOOST_SCOPE_EXIT_ALL(&) {
      m_fd = boost::none;
    };
#if defined(BOOST_POSIX_API)
    if (::close(*m_fd) == -1)
      BOOST_THROW_EXCEPTION(system_error("close")
                            << system_error::handle(*m_fd));
#elif defined(BOOST_WINDOWS_API)
    if (!::CloseHandle(*m_fd))
      BOOST_THROW_EXCEPTION(system_error("CloseHandle")
                            << system_error::handle(*m_fd));
#else
#error Unknown platform
#endif
  }
  BOOST_ASSERT(!m_fd);
}

void handle::close_no_except() noexcept {
  try {
    close();
  } catch (...) {
    // ignore
  }
}

handle handle::dup() const {
  if (!*this) BOOST_THROW_EXCEPTION(handle_is_closed_error());
#if defined(BOOST_POSIX_API)
  const implementation fd = ::fcntl(**this, F_DUPFD_CLOEXEC, 0);
  if (fd < 0)
    BOOST_THROW_EXCEPTION(system_error("fcntl")
                          << system_error::handle(**this));
#elif defined(BOOST_WINDOWS_API)
  implementation fd;
  if (!::DuplicateHandle(::GetCurrentProcess(), **this, ::GetCurrentProcess(),
                         &fd, 0, /* not inheritable */ false,
                         DUPLICATE_SAME_ACCESS)) {
    BOOST_THROW_EXCEPTION(system_error("DuplicateHandle")
                          << system_error::handle(**this));
  }
#endif
  return handle(fd);
}

#if defined(BOOST_POSIX_API)
handle handle::dup2(const implementation new_fd) const {
  if (!*this) BOOST_THROW_EXCEPTION(handle_is_closed_error());
  const implementation fd = ::dup3(**this, new_fd, O_CLOEXEC);
  if (fd < 0)
    BOOST_THROW_EXCEPTION(system_error("dup3")
                          << system_error::handle(**this)
                          << system_error::new_handle(new_fd));
  return handle(fd);
}
#endif

bool handle::inheritable() const {
#if defined(BOOST_POSIX_API)
  return ::fcntl(**this, F_GETFL) & FD_CLOEXEC;
#elif defined(BOOST_WINDOWS_API)
  DWORD flags;
  if (!::GetHandleInformation(**this, &flags))
    BOOST_THROW_EXCEPTION(system_error("GetHandleInformation")
                          << system_error::handle(**this));
  return flags & HANDLE_FLAG_INHERIT;
#else
#error Unknown platform
#endif
}

void handle::set_inheritable(const bool inheritable) {
#if defined(BOOST_POSIX_API)
  int flags = ::fcntl(**this, F_GETFL);
  if (inheritable) {
    flags &= ~FD_CLOEXEC;
  } else {
    flags |= FD_CLOEXEC;
  }
  if (::fcntl(**this, F_SETFD, flags) == -1)
    BOOST_THROW_EXCEPTION(system_error("fcntl")
                          << system_error::handle(**this));
#elif defined(BOOST_WINDOWS_API)
  if (!::SetHandleInformation(**this, HANDLE_FLAG_INHERIT,
                              inheritable ? HANDLE_FLAG_INHERIT : 0)) {
    BOOST_THROW_EXCEPTION(system_error("SetHandleInformation")
                          << system_error::handle(**this));
  }
#else
#error Unknown platform
#endif
}

namespace {
#if defined(BOOST_POSIX_API)
handle sys_open(const boost::filesystem::path &path, const int flags) {
  const int fd = ::open(path.string().c_str(), flags);
  if (fd < 0)
    BOOST_THROW_EXCEPTION(system_error("open") << system_error::path(path)
                                               << system_error::flags(flags));
  return file::handle(fd);
}

handle sys_open(const boost::filesystem::path &path, const int flags,
                const mode_t mode) {
  const int fd = ::open(path.string().c_str(), flags, mode);
  if (fd < 0)
    BOOST_THROW_EXCEPTION(system_error("open") << system_error::path(path)
                                               << system_error::flags(flags)
                                               << system_error::mode(mode));
  return file::handle(fd);
}
#elif defined(BOOST_WINDOWS_API)
template <typename... Args>
handle sys_open(const boost::filesystem::path &path, const DWORD access,
                const DWORD share_mode, SECURITY_ATTRIBUTES *const sec,
                const DWORD creation, const DWORD flags,
                const handle::implementation templ) {
  const handle::implementation fd = ::CreateFileW(
      path.c_str(), access, share_mode, sec, creation, flags, templ);
  if (fd == INVALID_HANDLE_VALUE)
    BOOST_THROW_EXCEPTION(system_error("CreateFile")
                          << system_error::path(path));
  return handle(fd);
}
#endif
}  // namespace

handle handle::open_null() {
#if defined(BOOST_POSIX_API)
  return open_read_write("/dev/null");
#elif defined(BOOST_WINDOWS_API)
  return open_read("NUL");
#else
#error Unknown platform
#endif
}

handle handle::open_read(const boost::filesystem::path &path) {
#if defined(BOOST_POSIX_API)
  return sys_open(path, O_RDONLY | O_CLOEXEC);
#elif defined(BOOST_WINDOWS_API)
  return sys_open(path, GENERIC_READ, 0, nullptr, OPEN_EXISTING, 0, nullptr);
#else
#error Unknown platform
#endif
}

handle handle::open_write(const boost::filesystem::path &path) {
#if defined(BOOST_POSIX_API)
  return sys_open(path, O_WRONLY | O_CREAT | O_TRUNC | O_CLOEXEC, 0666);
#elif defined(BOOST_WINDOWS_API)
  return sys_open(path, GENERIC_WRITE, 0, nullptr, OPEN_ALWAYS, 0, nullptr);
#else
#error Unknown platform
#endif
}

handle handle::open_read_write(const boost::filesystem::path &path) {
#if defined(BOOST_POSIX_API)
  return sys_open(path, O_RDWR | O_CREAT | O_CLOEXEC, 0666);
#elif defined(BOOST_WINDOWS_API)
  return sys_open(path, GENERIC_READ | GENERIC_WRITE, 0, nullptr, OPEN_ALWAYS,
                  0, nullptr);
#else
#error Unknown platform
#endif
}

handle handle::open_append(const boost::filesystem::path &path) {
#if defined(BOOST_POSIX_API)
  return sys_open(path, O_WRONLY | O_APPEND | O_CREAT | O_CLOEXEC);
#elif defined(BOOST_WINDOWS_API)
  return sys_open(path, FILE_APPEND_DATA, 0, nullptr, OPEN_ALWAYS, 0,
                  nullptr);
#else
#error Unknown platform
#endif
}

handle handle::std_input() {
#if defined(BOOST_POSIX_API)
  return handle(STDIN_FILENO, false);
#elif defined(BOOST_WINDOWS_API)
  return handle(::GetStdHandle(STD_INPUT_HANDLE), false);
#else
#error Unknown platform
#endif
}
handle handle::std_output() {
#if defined(BOOST_POSIX_API)
  return handle(STDOUT_FILENO, false);
#elif defined(BOOST_WINDOWS_API)
  return handle(::GetStdHandle(STD_OUTPUT_HANDLE), false);
#else
#error Unknown platform
#endif
}
handle handle::std_error() {
#if defined(BOOST_POSIX_API)
  return handle(STDERR_FILENO, false);
#elif defined(BOOST_WINDOWS_API)
  return handle(::GetStdHandle(STD_ERROR_HANDLE), false);
#else
#error Unknown platform
#endif
}

}  // namespace file
}  // namespace process
}  // namespace bunsan
