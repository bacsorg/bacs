#pragma once

#include <grpc++/server.h>
#include <grpc++/support/status.h>

#include <type_traits>

namespace bunsan::rpc {

namespace detail {
template <typename Input, typename Output, bool IsReader, bool IsWriter>
class context {
  static_assert(!IsReader || !IsWriter);

 public:
  context() = delete;
  context(const context &) = default;
  context &operator=(const context &) = default;

  context(grpc::ServerContext *ctx, Input *input, Output *output)
      : m_context(ctx), m_input(input), m_output(output) {}

  template <typename T>
  context authorize(T *this_, grpc::Status (T::*auth)()) const {
    if (!m_status.ok()) return *this;
    m_status = (this_->*auth)();
    return *this;
  }

  template <typename T>
  context authorize(const T *this_, grpc::Status (T::*auth)() const) const {
    if (!m_status.ok()) return *this;
    m_status = (this_->*auth)();
    return *this;
  }

  template <typename T>
  context authorize(T *this_,
                    grpc::Status (T::*auth)(grpc::ServerContext *ctx)) const {
    if (!m_status.ok()) return *this;
    m_status = (this_->*auth)(m_context);
    return *this;
  }

  template <typename T>
  context authorize(const T *this_,
                    grpc::Status (T::*auth)(grpc::ServerContext *ctx)
                        const) const {
    if (!m_status.ok()) return *this;
    m_status = (this_->*auth)(m_context);
    return *this;
  }

  template <typename T>
  context authorize(T *this_, grpc::Status (T::*auth)(grpc::ServerContext *ctx,
                                                      Input *input)) const {
    if (!m_status.ok()) return *this;
    m_status = (this_->*auth)(m_context, m_input);
    return *this;
  }

  template <typename T>
  context authorize(const T *this_,
                    grpc::Status (T::*auth)(grpc::ServerContext *ctx,
                                            Input *input) const) const {
    if (!m_status.ok()) return *this;
    m_status = (this_->*auth)(m_context, m_input);
    return *this;
  }

  template <typename Auth>
  context authorize(const Auth &auth) const {
    if (!m_status.ok()) return *this;
    if constexpr (std::is_invocable_r_v<grpc::Status, Auth>) {
      m_status = auth();
    } else if constexpr (std::is_invocable_r_v<grpc::Status, Auth,
                                               grpc::ServerContext *>) {
      m_status = auth(m_context);
    } else {
      m_status = auth(m_context, m_input);
      static_assert(std::is_invocable_r_v<grpc::Status, Auth,
                                          grpc::ServerContext *, Input *>);
    }
    return *this;
  }

  template <typename Body>
  context run(Body &&body) const {
    if constexpr (std::is_invocable_r_v<grpc::Status, Body>) {
      return run_status_body(body);
    } else if constexpr (!IsWriter && std::is_invocable_r_v<Output, Body>) {
      return run_response_body(body);
    } else {
      static_assert(std::is_void_v<std::invoke_result_t<Body>>);
      return run_void_body(body);
    }
  }

  grpc::Status unwrap() const { return m_status; }

 private:
  template <typename F>
  context run_status_body(F &&f) const {
    if (!m_status.ok()) return *this;
    try {
      m_status = f();
    } catch (std::exception &e) {
      m_status = grpc::Status(grpc::UNKNOWN, e.what());
    } catch (...) {
      m_status = grpc::Status(grpc::UNKNOWN, "Unknown");
    }
    return *this;
  }

  template <typename F>
  context run_void_body(F &&f) const {
    return run_status_body([f] {
      f();
      return grpc::Status::OK;
    });
  }

  template <typename F>
  context run_response_body(F &&f) const {
    return run_status_body([this, f] {
      *m_output = f();
      return grpc::Status::OK;
    });
  }

 private:
  grpc::ServerContext *m_context = nullptr;
  Input *m_input = nullptr;
  Output *m_output = nullptr;
  mutable grpc::Status m_status = grpc::Status::OK;
};
}  // namespace detail

template <typename Request, typename Response>
auto wrap(grpc::ServerContext *ctx, const Request *request,
          Response *response) {
  static_assert(std::is_convertible_v<Request *, google::protobuf::Message *>);
  static_assert(std::is_convertible_v<Response *, google::protobuf::Message *>);
  return detail::context<const Request, Response, false, false>(ctx, request,
                                                                response);
}

template <typename Request, typename Response>
auto wrap(grpc::ServerContext *ctx, const Request *request,
          grpc::ServerWriter<Response> *writer) {
  static_assert(std::is_convertible_v<Request *, google::protobuf::Message *>);
  return detail::context<const Request, grpc::ServerWriter<Response>, false,
                         true>(ctx, request, writer);
}

template <typename Request, typename Response>
auto wrap(grpc::ServerContext *ctx, grpc::ServerReader<Request> *reader,
          Response *response) {
  static_assert(std::is_convertible_v<Response *, google::protobuf::Message *>);
  return detail::context<grpc::ServerReader<Request>, Response, true, false>(
      ctx, reader, response);
}

template <typename Request, typename Response>
void wrap(grpc::ServerContext * /*ctx*/,
          grpc::ServerReaderWriter<Response, Request> * /*stream*/) {
  // not supported
}

}  // namespace bunsan::rpc
