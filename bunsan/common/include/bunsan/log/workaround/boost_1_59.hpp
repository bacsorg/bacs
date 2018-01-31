#pragma once

// https://github.com/boostorg/log/commit/7da193fde1a9c1bc925ee980339f4df2e1a66fa7

#include <boost/log/sources/record_ostream.hpp>

namespace boost {

BOOST_LOG_OPEN_NAMESPACE

template <typename CharT>
class basic_record_ostream;

namespace aux {

template <typename StreamT, typename R>
struct enable_if_record_ostream {};
template <typename CharT, typename R>
struct enable_if_record_ostream<basic_record_ostream<CharT>, R> {
  typedef R type;
};

}  // namespace aux

template <typename StreamT, typename T>
inline
    typename boost::log::aux::enable_if_record_ostream<StreamT, StreamT&>::type
    operator<<(StreamT& strm, T const& value) {
  typedef basic_formatting_ostream<typename StreamT::char_type>
      formatting_ostream_type;
  static_cast<formatting_ostream_type&>(strm) << value;
  return strm;
}

template <typename StreamT, typename T>
inline
    typename boost::log::aux::enable_if_record_ostream<StreamT, StreamT&>::type
    operator<<(StreamT& strm, T& value) {
  typedef basic_formatting_ostream<typename StreamT::char_type>
      formatting_ostream_type;
  static_cast<formatting_ostream_type&>(strm) << value;
  return strm;
}

#if !defined(BOOST_NO_CXX11_RVALUE_REFERENCES)

template <typename StreamT, typename T>
inline
    typename boost::log::aux::enable_if_record_ostream<StreamT, StreamT&>::type
    operator<<(StreamT&& strm, T const& value) {
  typedef basic_formatting_ostream<typename StreamT::char_type>
      formatting_ostream_type;
  static_cast<formatting_ostream_type&>(strm) << value;
  return strm;
}

template <typename StreamT, typename T>
inline
    typename boost::log::aux::enable_if_record_ostream<StreamT, StreamT&>::type
    operator<<(StreamT&& strm, T& value) {
  typedef basic_formatting_ostream<typename StreamT::char_type>
      formatting_ostream_type;
  static_cast<formatting_ostream_type&>(strm) << value;
  return strm;
}

#endif  // !defined(BOOST_NO_CXX11_RVALUE_REFERENCES)

BOOST_LOG_CLOSE_NAMESPACE  // namespace log

}  // namespace boost
