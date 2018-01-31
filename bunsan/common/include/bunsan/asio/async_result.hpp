#pragma once

#include <boost/asio/async_result.hpp>

#define BUNSAN_ASIO_INITFN_HANDLER_TYPE(h, sig) \
  typename ::boost::asio::handler_type<h, sig>::type

#define BUNSAN_ASIO_INITFN_RESULT_TYPE(h, sig)                          \
  typename ::boost::asio::async_result<BUNSAN_ASIO_INITFN_HANDLER_TYPE( \
      h, sig)>::type

#define BUNSAN_ASIO_INITFN_BEGIN(h, handler, sig, real_handler, result)        \
  BUNSAN_ASIO_INITFN_HANDLER_TYPE(h, sig)                                      \
  real_handler(std::forward<h>(handler));                                      \
  ::boost::asio::async_result<BUNSAN_ASIO_INITFN_HANDLER_TYPE(h, sig)> result( \
      real_handler);

#define BUNSAN_ASIO_INITFN_END(result) return result.get();
