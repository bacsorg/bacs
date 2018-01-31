#include <bunsan/interprocess/sync/file_guard.hpp>

#include <bunsan/filesystem/error.hpp>

#include <boost/assert.hpp>
#include <boost/filesystem/operations.hpp>

#include <cstdio>

namespace bunsan {
namespace interprocess {

file_guard::file_guard(const boost::filesystem::path &path) : m_path(path) {
  try {
    FILE *const file = std::fopen(path.string().c_str(), "wx");
    if (file) {
      BOOST_VERIFY(fclose(file) == 0);
    } else if (errno == EEXIST) {
      BOOST_THROW_EXCEPTION(file_guard_locked_error()
                            << file_guard_locked_error::lock_path(path));
    } else {
      BOOST_THROW_EXCEPTION(filesystem::system_error("open")
                            << filesystem::system_error::path(path));
    }
  } catch (file_guard_locked_error &) {
    throw;
  } catch (std::exception &) {
    BOOST_THROW_EXCEPTION(file_guard_create_error() << enable_nested_current());
  }
}

file_guard::file_guard(file_guard &&guard) noexcept {
  swap(guard);
  guard.remove();
}

file_guard &file_guard::operator=(file_guard &&guard) noexcept {
  swap(guard);
  guard.remove();
  return *this;
}

file_guard::operator bool() const noexcept { return static_cast<bool>(m_path); }

void file_guard::swap(file_guard &guard) noexcept {
  using boost::swap;
  swap(m_path, guard.m_path);
}

void file_guard::remove() {
  try {
    if (m_path) BOOST_VERIFY(boost::filesystem::remove(m_path.get()));
    m_path.reset();
  } catch (std::exception &) {
    BOOST_THROW_EXCEPTION(file_guard_remove_error() << enable_nested_current());
  }
}

file_guard::~file_guard() {
  try {
    remove();
  } catch (...) {
    // ignore
  }
}

}  // namespace interprocess
}  // namespace bunsan
