#pragma once

#include <bunsan/filesystem/error.hpp>

#include <boost/filesystem/fstream.hpp>
#include <boost/scope_exit.hpp>

namespace bunsan {
namespace filesystem {

template <typename Fstream, std::ios_base::openmode BaseOpenmode>
class basic_stream;

namespace detail {
template <bool enable>
struct flusher {
  template <typename T>
  static void call(T &obj) {
    obj.flush();
  }
};

template <>
struct flusher<false> {
  template <typename T>
  static void call(T &) {}
};

template <typename Fstream, std::ios_base::openmode BaseOpenmode>
const boost::filesystem::path &path(
    basic_stream<Fstream, BaseOpenmode> &stream) {
  return stream.m_path;
}

template <typename Fstream, std::ios_base::openmode BaseOpenmode>
std::ios_base::openmode openmode(basic_stream<Fstream, BaseOpenmode> &stream) {
  return stream.m_openmode;
}
}  // namespace detail

template <typename... Args>
using basic_filebuf = boost::filesystem::basic_filebuf<Args...>;

template <typename Fstream, std::ios_base::openmode BaseOpenmode>
class basic_stream : public Fstream {
 public:
  basic_stream() { this->exceptions(std::ios_base::badbit); }

  explicit basic_stream(const boost::filesystem::path &path)
      : Fstream(path), m_path(path), m_openmode(BaseOpenmode) {
    if (!this->is_open())
      BOOST_THROW_EXCEPTION(system_error("open")
                            << error::path(m_path)
                            << error::openmode(m_openmode));
    this->exceptions(std::ios_base::badbit);
  }

  basic_stream(const boost::filesystem::path &path,
               const std::ios_base::openmode mode)
      : Fstream(path, mode), m_path(path), m_openmode(mode | BaseOpenmode) {
    if (!this->is_open())
      BOOST_THROW_EXCEPTION(system_error("open")
                            << error::path(m_path)
                            << error::openmode(m_openmode));
    this->exceptions(std::ios_base::badbit);
  }

  void open(const boost::filesystem::path &path) {
    m_path = path;
    m_openmode = BaseOpenmode;
    Fstream::open(m_path);
    if (!this->is_open())
      BOOST_THROW_EXCEPTION(system_error("open")
                            << error::path(m_path)
                            << error::openmode(m_openmode));
  }

  void open(const boost::filesystem::path &path,
            const std::ios_base::openmode mode) {
    m_path = path;
    m_openmode = mode | BaseOpenmode;
    Fstream::open(m_path, m_openmode);
    if (!this->is_open())
      BOOST_THROW_EXCEPTION(system_error("open")
                            << error::path(m_path)
                            << error::openmode(m_openmode));
  }

  using Fstream::exceptions;

  void exceptions(const std::ios_base::iostate except) {
    Fstream::exceptions(except | std::ios_base::badbit);
  }

  void close() {
    try {
      // note: if we don't call flush before close(),
      // close() will override errno
      // note: flush is only enabled for output streams
      detail::flusher<BaseOpenmode & std::ios_base::out>::call(*this);

      // note: close() sets failbit on any error
      const std::ios_base::iostate state = exceptions();
      BOOST_SCOPE_EXIT_ALL(this, state) { exceptions(state); };  //< rollback
      // exceptions() should not throw here
      this->clear(this->rdstate() & ~std::ios_base::failbit);
      exceptions(state | std::ios_base::failbit);
      Fstream::close();
    } catch (std::ios_base::failure &) {
      BOOST_THROW_EXCEPTION(system_error("close")
                            << enable_nested_current() << error::path(m_path)
                            << error::openmode(m_openmode));
    }
  }

  template <typename Fstream_, std::ios_base::openmode BaseOpenmode_>
  friend const boost::filesystem::path &detail::path(
      basic_stream<Fstream_, BaseOpenmode_> &stream);

  template <typename Fstream_, std::ios_base::openmode BaseOpenmode_>
  friend std::ios_base::openmode detail::openmode(
      basic_stream<Fstream_, BaseOpenmode_> &stream);

 private:
  boost::filesystem::path m_path;
  std::ios_base::openmode m_openmode = BaseOpenmode;
};

template <typename CharT, typename Traits = std::char_traits<CharT>>
using basic_ifstream =
    basic_stream<boost::filesystem::basic_ifstream<CharT, Traits>,
                 std::ios_base::in>;

template <typename CharT, typename Traits = std::char_traits<CharT>>
using basic_ofstream =
    basic_stream<boost::filesystem::basic_ofstream<CharT, Traits>,
                 std::ios_base::out>;

template <typename CharT, typename Traits = std::char_traits<CharT>>
using basic_fstream =
    basic_stream<boost::filesystem::basic_fstream<CharT, Traits>,
                 std::ios_base::openmode{}>;

using filebuf = basic_filebuf<char>;
using ifstream = basic_ifstream<char>;
using ofstream = basic_ofstream<char>;
using fstream = basic_fstream<char>;

using wfilebuf = basic_filebuf<wchar_t>;
using wifstream = basic_ifstream<wchar_t>;
using wofstream = basic_ofstream<wchar_t>;
using wfstream = basic_fstream<wchar_t>;

/*!
 * \brief Wrapper for exception-safe usage of bunsan::filesystem::fstream.
 *
 * \code{.cpp}
 * bunsan::filesystem::ofstream fout(path);
 * BUNSAN_FILESYSTEM_FSTREAM_WRAP_BEGIN(fout)
 * {
 *     fout << "data";
 * }
 * BUNSAN_FILESYSTEM_FSTREAM_WRAP_END(fout)
 * fout.close();
 * \endcode
 */
#define BUNSAN_FILESYSTEM_FSTREAM_WRAP_BEGIN(FSTREAM) try

#define BUNSAN_FILESYSTEM_FSTREAM_WRAP_END(FSTREAM)                          \
  catch (::boost::exception &) { /* do not catch exceptions thrown by nested \
                                    wrappers */                              \
    throw;                                                                   \
  } catch (::std::ios_base::failure &) {                                     \
    BOOST_THROW_EXCEPTION(                                                   \
        ::bunsan::filesystem::system_error()                                 \
        << ::bunsan::enable_nested_current()                                 \
        << ::bunsan::filesystem::error::path(                                \
               ::bunsan::filesystem::detail::path(FSTREAM))                  \
        << ::bunsan::filesystem::error::openmode(                            \
               ::bunsan::filesystem::detail::openmode(FSTREAM)));            \
  }

}  // namespace filesystem
}  // namespace bunsan
