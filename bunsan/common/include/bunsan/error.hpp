#pragma once

#include <bunsan/error/manip.hpp>
#include <bunsan/runtime/demangle.hpp>
#include <bunsan/runtime/stacktrace.hpp>

#include <boost/exception/all.hpp>
#include <boost/version.hpp>

#include <exception>

// see bunsan::error::info_name()
#if BOOST_VERSION < 105400
#include <typeinfo>
#endif

namespace bunsan {

struct error : virtual std::exception, virtual boost::exception {
  /*!
   * \brief Create error object with stacktrace tag.
   *
   * \see stacktrace
   */
  error();

  /*!
   * \brief Create error object with message and stacktrace tags.
   *
   * \see message
   * \see stacktrace
   */
  error(const std::string &message_);

  const char *what() const noexcept override;

  /*!
   * \tparam ErrorInfo boost::error_info instantiation
   * \return pointer to data associated with ErrorInfo
   */
  template <typename ErrorInfo>
  const typename ErrorInfo::value_type *get() const noexcept {
    return boost::get_error_info<ErrorInfo>(*this);
  }

  /*!
   * \copydoc get
   */
  template <typename ErrorInfo>
  typename ErrorInfo::value_type *get() noexcept {
    return boost::get_error_info<ErrorInfo>(*this);
  }

  /// Human readable error message
  using message = boost::error_info<struct tag_message, std::string>;

  using stacktrace =
      boost::error_info<struct tag_stacktrace, runtime::stacktrace>;

  using nested_exception = boost::errinfo_nested_exception;

  template <typename Tag, typename T>
  static std::string info_name(const boost::error_info<Tag, T> &x) {
#if BOOST_VERSION >= 105400
    return boost::error_info_name(x);
#else
    (void)x;
    return runtime::demangle(typeid(Tag *).name());
#endif
  }
};

template <>
class error_manip<struct tag_enable_stacktrace> {
 public:
  /*!
   * \brief Enable stack trace if not provided.
   *
   * \param skip number of nested function to omit in stack trace
   */
  explicit error_manip(const std::size_t skip = 0) : m_skip(skip) {}

  void operator()(const boost::exception &e) const;

 private:
  std::size_t m_skip;
};
using enable_stacktrace = error_manip<tag_enable_stacktrace>;

template <>
class error_manip<struct tag_enable_nested> {
 public:
  explicit error_manip(const boost::exception_ptr &ptr) : m_ptr(ptr) {}

  void operator()(const boost::exception &e) const;

 private:
  boost::exception_ptr m_ptr;
};
using enable_nested = error_manip<tag_enable_nested>;

template <>
struct error_manip<struct tag_enable_nested_current> {
  void operator()(const boost::exception &e) const;
};
using enable_nested_current = error_manip<tag_enable_nested_current>;

}  // namespace bunsan
