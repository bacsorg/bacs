#include <bunsan/tempfile.hpp>

#include <bunsan/filesystem/error.hpp>
#include <bunsan/log/trivial.hpp>

#include <boost/filesystem/operations.hpp>
#include <boost/scope_exit.hpp>
#include <boost/utility/swap.hpp>

#include <cstdio>

namespace bunsan {

const boost::filesystem::path tempfile::default_model = "%%%%-%%%%-%%%%-%%%%";

// constructors

tempfile::tempfile() : m_do_auto_remove(false) {}

tempfile::tempfile(const boost::filesystem::path &path, bool do_auto_remove)
    : m_path(path), m_do_auto_remove(do_auto_remove) {}

tempfile::tempfile(tempfile &&tmp) noexcept { swap(tmp); }

tempfile &tempfile::operator=(tempfile &&tmp) noexcept {
  swap(tmp);
  return *this;
}

// swap

void tempfile::swap(tempfile &tmp) noexcept {
  using boost::swap;
  swap(m_path, tmp.m_path);
  swap(m_do_auto_remove, tmp.m_do_auto_remove);
}

// path accessors

const boost::filesystem::path &tempfile::path() const { return m_path; }

boost::filesystem::path::string_type tempfile::native() const {
  return m_path.native();
}

std::string tempfile::generic_string() const { return m_path.generic_string(); }

std::string tempfile::string() const { return m_path.string(); }

bool tempfile::auto_remove() const { return m_do_auto_remove; }

void tempfile::auto_remove(bool do_auto_remove) {
  m_do_auto_remove = do_auto_remove;
}

tempfile::~tempfile() {
  if (m_do_auto_remove && !m_path.empty()) {
    try {
      BUNSAN_LOG_TRACE << "Removing tempfile " << m_path;
      if (boost::filesystem::exists(m_path))
        boost::filesystem::remove_all(m_path);
    } catch (std::exception &e) {
      BUNSAN_LOG_ERROR << "Unable to remove tempfile " << m_path << ": "
                       << e.what();
    } catch (...) {
      BUNSAN_LOG_ERROR << "Unable to remove tempfile " << m_path << ": unknown";
    }
  }
}

#define BUNSAN_TEMPFILE_OBJECT(TYPE)                                           \
  tempfile tempfile::TYPE##_in_directory(                                      \
      const boost::filesystem::path &directory) {                              \
    return TYPE##_from_model(directory / default_model);                       \
  }                                                                            \
  tempfile tempfile::TYPE##_in_tempdir() {                                     \
    return TYPE##_in_directory(boost::filesystem::temp_directory_path());      \
  }                                                                            \
  tempfile tempfile::TYPE##_in_tempdir(const boost::filesystem::path &model) { \
    return TYPE##_from_model(boost::filesystem::temp_directory_path() /        \
                             model);                                           \
  }                                                                            \
  tempfile tempfile::TYPE##_from_model(const boost::filesystem::path &model) { \
    constexpr std::size_t TRIES = 32;                                          \
    for (std::size_t i = 0; i < TRIES; ++i) {                                  \
      tempfile tmp(boost::filesystem::unique_path(model));                     \
      if (create_##TYPE(tmp.path())) {                                         \
        return tmp;                                                            \
      } else {                                                                 \
        tmp.auto_remove(false);                                                \
      }                                                                        \
    }                                                                          \
    BOOST_THROW_EXCEPTION(                                                     \
        unable_to_create_unique_temp_##TYPE()                                  \
        << unable_to_create_unique_temp_##TYPE::tries(TRIES)                   \
        << unable_to_create_unique_temp_##TYPE::model(model));                 \
  }

BUNSAN_TEMPFILE_OBJECT(regular_file)
BUNSAN_TEMPFILE_OBJECT(directory)

bool tempfile::create_regular_file(const boost::filesystem::path &path) {
  FILE *file = nullptr;
  BOOST_SCOPE_EXIT_ALL(&file) {
    if (file) fclose(file);
  };
  file = fopen(path.string().c_str(), "wx");
  if (file) {
    return true;
  } else {
    if (errno == EEXIST) {
      return false;
    } else {
      BOOST_THROW_EXCEPTION(filesystem::system_error("fopen"));
    }
  }
}

bool tempfile::create_directory(const boost::filesystem::path &path) {
  return boost::filesystem::create_directory(path);
}

}  // namespace bunsan
