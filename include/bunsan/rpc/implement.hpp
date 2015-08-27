#pragma once

#include <grpc++/status.h>

#include <type_traits>

namespace bunsan {
namespace rpc {
namespace detail {

template <typename F>
grpc::Status implement_status(F &&f) {
  try {
    return f();
  } catch (std::exception &e) {
    return grpc::Status(grpc::UNKNOWN, e.what());
  } catch (...) {
    return grpc::Status(grpc::UNKNOWN, "Unknown");
  }
}

template <typename Response, typename F>
grpc::Status implement_response(Response &&response, F &&f) {
  return implement_status([response, &f] {
    *response = f();
    return grpc::Status::OK;
  });
}

}  // namespace detail
}  // namespace bunsan
}  // namespace bunsan

#define BUNSAN_RPC_IMPLEMENT_STATUS(CONTEXT, REQUEST, RESPONSE, BODY) \
  return ::bunsan::rpc::detail::implement_status(                     \
      [this, CONTEXT, REQUEST, RESPONSE]() -> ::grpc::Status BODY);

#define BUNSAN_RPC_IMPLEMENT_RESPONSE(CONTEXT, REQUEST, RESPONSE, BODY) \
  return ::bunsan::rpc::detail::implement_response(                     \
      RESPONSE, [this, CONTEXT, REQUEST]()                              \
                    -> std::remove_reference_t<decltype(*RESPONSE)> BODY);
