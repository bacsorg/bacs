#pragma once

#include <bunsan/error.hpp>
#include <bunsan/runtime/demangle.hpp>
#include <bunsan/system_error.hpp>

#include <boost/preprocessor/seq/for_each.hpp>
#include <boost/preprocessor/variadic/to_seq.hpp>
#include <boost/system/system_error.hpp>

#include <ios>
#include <stdexcept>
#include <system_error>
#include <type_traits>

namespace bunsan {

namespace exception_detail {
using original_exception =
    boost::error_info<struct tag_original_exception, boost::exception_ptr>;
using original_what = boost::error_info<struct tag_original_what, std::string>;

/// \todo use std::exception_ptr
template <typename T>
struct error_info_injector : T, boost::exception {
  explicit error_info_injector(const T &e, const std::size_t skip = 0) : T(e) {
    // +1 for current function
    (*this) << error::stacktrace(runtime::stacktrace::get(skip + 1));
    (*this) << original_exception(boost::current_exception());
  }

  const char *what() const noexcept override {
    if (!boost::get_error_info<original_what>(*this))
      (*this) << original_what(T::what());
    return boost::diagnostic_information_what(*this);
  }
};
}  // namespace exception_detail

template <typename T>
typename std::enable_if<!std::is_base_of<boost::exception, T>::value,
                        exception_detail::error_info_injector<T>>::type
enable_error_info(const T &obj) {
  return exception_detail::error_info_injector<T>(obj, 1);
}

}  // namespace bunsan

/*!
 * \brief Begin exceptions wrapping statement
 *
 * These macros add stack trace and other useful info
 * to thrown exception. If exception is derived from boost::exception
 * it's type is preserved. Otherwise it's type is unspecified derived from
 * boost::exception and caught exception type.
 *
 * Predefined list of exceptions to catch
 * consists of exceptions from <stdexcept>,
 * <system_error> and <boost/system/system_error.hpp>.
 * Additional exceptions may be specified by user via
 * BUNSAN_EXCEPTIONS_WRAP_END_EXCEPT and
 * BUNSAN_EXCEPTIONS_WRAP_END_ERROR_INFO_EXCEPT.
 *
 * \code{.cpp}
 * BUNSAN_EXCEPTIONS_WRAP_BEGIN()
 * {
 *     throw std::runtime_error("hello, world");
 * }
 * BUNSAN_EXCEPTIONS_WRAP_END()
 * \endcode
 */
#define BUNSAN_EXCEPTIONS_WRAP_BEGIN() try

#define BUNSAN_EXCEPTIONS_WRAP_WRAPPER(r, ERROR_INFO, ERROR)          \
  catch (ERROR & e) {                                                 \
    BOOST_THROW_EXCEPTION(::bunsan::enable_error_info(e) ERROR_INFO); \
  }

#define BUNSAN_EXCEPTIONS_WRAP_END_(ERROR_INFO, ...)                          \
  catch (::boost::exception & e) {                                            \
    if (!::boost::get_error_info<::bunsan::error::stacktrace>(e))             \
      e << ::bunsan::error::stacktrace(::bunsan::runtime::stacktrace::get()); \
    if (!::boost::get_error_info<::boost::throw_function>(e))                 \
      e << ::boost::throw_function(BOOST_CURRENT_FUNCTION);                   \
    if (!::boost::get_error_info<::boost::throw_file>(e))                     \
      e << ::boost::throw_file(__FILE__);                                     \
    if (!::boost::get_error_info<::boost::throw_line>(e))                     \
      e << ::boost::throw_line(__LINE__);                                     \
    (void)(e ERROR_INFO);                                                     \
    throw;                                                                    \
  }                                                                           \
  catch (::std::ios_base::failure & e) {                                      \
    /* note: BUNSAN_FILESYSTEM_FSTREAM_WRAP is used to wrap that type */      \
    /* TODO: should be removed when std::ios_base::failure is derived from    \
     * std::system_error */                                                   \
    throw;                                                                    \
  }                                                                           \
  catch (::std::bad_alloc &) {                                                \
    /* should not be wrapped, there are no enough memory */                   \
    throw;                                                                    \
  }                                                                           \
  BOOST_PP_SEQ_FOR_EACH(BUNSAN_EXCEPTIONS_WRAP_WRAPPER, ERROR_INFO,           \
                        BOOST_PP_VARIADIC_TO_SEQ(__VA_ARGS__))

/// Predefined list of exceptions.
#define BUNSAN_EXCEPTIONS_WRAP_LIST                                       \
  ::std::domain_error, ::std::invalid_argument, ::std::length_error,      \
      ::std::out_of_range, ::std::logic_error, ::std::range_error,        \
      ::std::overflow_error, ::std::underflow_error, ::std::system_error, \
      ::boost::system::system_error, ::std::runtime_error, ::std::exception

/// Default footer.
#define BUNSAN_EXCEPTIONS_WRAP_END() \
  BUNSAN_EXCEPTIONS_WRAP_END_(, BUNSAN_EXCEPTIONS_WRAP_LIST)

/// Catch exceptions specified as arguments before default list.
#define BUNSAN_EXCEPTIONS_WRAP_END_EXCEPT(...) \
  BUNSAN_EXCEPTIONS_WRAP_END_(, __VA_ARGS__, BUNSAN_EXCEPTIONS_WRAP_LIST)

/*!
 * \brief Add error info to wrapped exception.
 *
 * \param ERROR_INFO boost::error_info instance (streaming expression)
 *
 * \code{.cpp}
 * BUNSAN_EXCEPTIONS_WRAP_END_ERROR_INFO(
 *     bunsan::filesystem::error::source_path(path1) <<
 *     bunsan::filesystem::error::destination_path(path2))
 * \endcode
 */
#define BUNSAN_EXCEPTIONS_WRAP_END_ERROR_INFO(ERROR_INFO) \
  BUNSAN_EXCEPTIONS_WRAP_END_(<< ERROR_INFO, BUNSAN_EXCEPTIONS_WRAP_LIST)

/*!
 * \brief Add error info to wrapped exception + catch additional exceptions.
 *
 * \param ERROR_INFO boost::error_info instance (streaming expression)
 * \param __VA_ARGS__ additional exceptions to catch
 *
 * \code{.cpp}
 * BUNSAN_EXCEPTIONS_WRAP_END_ERROR_INFO_EXCEPT(
 *     bunsan::filesystem::error::source_path(path1) <<
 *     bunsan::filesystem::error::destination_path(path2),
 *     boost::filesystem::filesystem_error,
 *     another::exception::type)
 * \endcode
 */
#define BUNSAN_EXCEPTIONS_WRAP_END_ERROR_INFO_EXCEPT(ERROR_INFO, ...) \
  BUNSAN_EXCEPTIONS_WRAP_END_(<< ERROR_INFO, __VA_ARGS__,             \
                              BUNSAN_EXCEPTIONS_WRAP_LIST)
