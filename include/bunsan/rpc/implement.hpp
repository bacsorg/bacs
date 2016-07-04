#pragma once

#include <grpc++/support/status.h>

#include <type_traits>

namespace bunsan {
namespace rpc {
namespace detail {

template <typename Authorize, typename F>
grpc::Status implement_status(Authorize &&authorize, F &&f) {
  try {
    const grpc::Status auth = authorize();
    if (!auth.ok()) return auth;
  } catch (std::exception &e) {
    return grpc::Status(grpc::UNKNOWN, "Unable to authorize");
  } catch (...) {
    return grpc::Status(grpc::UNKNOWN, "Unable to authorize");
  }
  try {
    return f();
  } catch (std::exception &e) {
    return grpc::Status(grpc::UNKNOWN, e.what());
  } catch (...) {
    return grpc::Status(grpc::UNKNOWN, "Unknown");
  }
}

template <typename Authorize, typename Response, typename F>
grpc::Status implement_response(Authorize &&authorize, Response &&response,
                                F &&f) {
  return implement_status(authorize, [response, &f] {
    *response = f();
    return grpc::Status::OK;
  });
}

}  // namespace detail
}  // namespace rpc
}  // namespace bunsan

#define BUNSAN_RPC_IMPLEMENT_STATUS(CONTEXT, REQUEST, RESPONSE, AUTHORIZE, \
                                    BODY)                                  \
  return ::bunsan::rpc::detail::implement_status(                          \
      [this, CONTEXT, REQUEST, RESPONSE] {                                 \
    return AUTHORIZE(CONTEXT, REQUEST, RESPONSE);                          \
      },                                                                   \
      [this, CONTEXT, REQUEST, RESPONSE]() -> ::grpc::Status BODY);

#define BUNSAN_RPC_IMPLEMENT_RESPONSE(CONTEXT, REQUEST, RESPONSE, AUTHORIZE, \
                                      BODY)                                  \
  return ::bunsan::rpc::detail::implement_response(                          \
      [this, CONTEXT, REQUEST, RESPONSE] {                                   \
    return AUTHORIZE(CONTEXT, REQUEST, RESPONSE);                            \
      },                                                                     \
      RESPONSE, [this, CONTEXT, REQUEST]()                                   \
                    -> std::remove_reference_t<decltype(*RESPONSE)> BODY);
