#pragma once

#include <bunsan/rpc/error.hpp>

#include <boost/preprocessor/tuple/elem.hpp>
#include <boost/preprocessor/tuple/pop_front.hpp>
#include <boost/preprocessor/tuple/push_back.hpp>
#include <boost/preprocessor/tuple/rem.hpp>

#define BUNSAN_RPC_OVERLOAD_IMPL(NAME, RESPONSE, ARGUMENTS)             \
  RESPONSE response;                                                    \
  const auto status = NAME(BOOST_PP_TUPLE_REM() ARGUMENTS);             \
  if (!status.ok())                                                     \
    BOOST_THROW_EXCEPTION(::bunsan::rpc::error()                        \
                          << ::bunsan::rpc::error::rpc_status(status)); \
  return response;

#define BUNSAN_RPC_OVERLOAD_INVOKER(NAME, ARGS)                \
  BUNSAN_RPC_OVERLOAD_IMPL(NAME, BOOST_PP_TUPLE_ELEM(0, ARGS), \
                           BOOST_PP_TUPLE_POP_FRONT(ARGS))

#define BUNSAN_RPC_OVERLOAD(NAME, ...) \
  BUNSAN_RPC_OVERLOAD_INVOKER(         \
      NAME, BOOST_PP_TUPLE_PUSH_BACK((__VA_ARGS__), response))
