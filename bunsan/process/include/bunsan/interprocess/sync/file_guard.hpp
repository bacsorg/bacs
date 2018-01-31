#pragma once

#include <bunsan/error.hpp>

#include <boost/filesystem/path.hpp>
#include <boost/optional.hpp>

namespace bunsan {
namespace interprocess {

struct file_guard_error : virtual bunsan::error {
  using lock_path =
      boost::error_info<struct tag_lock_path, boost::filesystem::path>;
};

struct file_guard_create_error : virtual file_guard_error {};
struct file_guard_locked_error : virtual file_guard_create_error {};
struct file_guard_remove_error : virtual file_guard_error {};

class file_guard {
 public:
  /// Unlocked guard.
  file_guard() = default;

  /*!
   * \brief Create file at specified path.
   *
   * \throw file_guard_locked_error if file exists (type of file does not
   *matter)
   */
  explicit file_guard(const boost::filesystem::path &path);

  // move semantics
  file_guard(file_guard &&) noexcept;
  file_guard &operator=(file_guard &&) noexcept;

  // no copying
  file_guard(const file_guard &) = delete;
  file_guard &operator=(const file_guard &) = delete;

  explicit operator bool() const noexcept;

  void swap(file_guard &guard) noexcept;

  void remove();

  ~file_guard();

 private:
  boost::optional<boost::filesystem::path> m_path;
};

inline void swap(file_guard &a, file_guard &b) noexcept { a.swap(b); }

}  // namespace interprocess
}  // namespace bunsan
