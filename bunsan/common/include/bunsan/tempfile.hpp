#pragma once

#include <bunsan/error.hpp>

#include <boost/filesystem/path.hpp>

#include <string>

namespace bunsan {

struct tempfile_error : virtual error {
  using model = boost::error_info<struct tag_model, boost::filesystem::path>;
};
struct tempfile_create_error : virtual tempfile_error {
  using tries = boost::error_info<struct tag_tries, std::size_t>;
};
struct unable_to_create_unique_temp_directory : virtual tempfile_create_error {
};
struct unable_to_create_unique_temp_regular_file
    : virtual tempfile_create_error {};

/*!
 * \note Model is pattern with '%' symbols
 * that are replaced by random symbols from [0-9a-f]
 */
class tempfile {
 public:
  tempfile();

  /// Captures existing filesystem object
  explicit tempfile(const boost::filesystem::path &path,
                    bool do_auto_remove = true);
  tempfile(const tempfile &tmp) = delete;
  tempfile(tempfile &&tmp) noexcept;

  tempfile &operator=(const tempfile &tmp) = delete;
  tempfile &operator=(tempfile &&tmp) noexcept;

  // path access
  const boost::filesystem::path &path() const;
  boost::filesystem::path::string_type native() const;
  std::string generic_string() const;
  std::string string() const;

  bool auto_remove() const;
  void auto_remove(bool do_auto_remove_);

  ~tempfile();

  // swap
  void swap(tempfile &tmp) noexcept;

 public:
  static tempfile regular_file_in_directory(
      const boost::filesystem::path &directory);
  static tempfile directory_in_directory(
      const boost::filesystem::path &directory);

  static tempfile regular_file_in_tempdir();
  static tempfile directory_in_tempdir();

  static tempfile regular_file_in_tempdir(const boost::filesystem::path &model);
  static tempfile directory_in_tempdir(const boost::filesystem::path &model);

  static tempfile regular_file_from_model(const boost::filesystem::path &model);
  static tempfile directory_from_model(const boost::filesystem::path &model);

 private:
  /// \return false if exists
  static bool create_regular_file(const boost::filesystem::path &path);

  /// \return false if exists
  static bool create_directory(const boost::filesystem::path &path);

 private:
  static const boost::filesystem::path default_model;

 private:
  boost::filesystem::path m_path;
  bool m_do_auto_remove;
};

inline void swap(tempfile &a, tempfile &b) noexcept { a.swap(b); }

}  // namespace bunsan
