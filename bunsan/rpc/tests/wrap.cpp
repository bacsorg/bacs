#define BOOST_TEST_MODULE wrap
#include <boost/test/unit_test.hpp>

#include <turtle/mock.hpp>

#include <bunsan/rpc/test/service.grpc.pb.h>
#include <bunsan/rpc/wrap.hpp>

namespace bunsan::rpc::test {
// TODO use this in actual test
class MyService : public TestService::Service, mock::object {
 public:
  grpc::Status DoRpc(grpc::ServerContext *context, const Request *request,
                     Response *response) override {
    return wrap(context, request, response)
        .authorize(this, &MyService::VoidAuth)
        .run([=] { response->set_value(request->value()); })
        .unwrap();
  }

  grpc::Status DoRpcRStream(grpc::ServerContext *context,
                            const Request *request,
                            grpc::ServerWriter<Response> *writer) override {
    return wrap(context, request, writer)
        .authorize(this, &MyService::VoidAuth)
        .run([=] {
          for (int i = 0; i < request->value(); ++i) {
            Response rsp;
            rsp.set_value(i);
            if (!writer->Write(rsp)) return grpc::Status::CANCELLED;
          }
          return grpc::Status::OK;
        })
        .unwrap();
  }

  grpc::Status DoRpcWStream(grpc::ServerContext *context,
                            grpc::ServerReader<Request> *reader,
                            Response *response) override {
    return wrap(context, reader, response)
        .authorize(this, &MyService::VoidAuth)
        .run([=] {
          Request req;
          Response rsp;
          while (reader->Read(&req)) {
            rsp.set_value(rsp.value() + req.value());
          }
          return rsp;
        })
        .unwrap();
  }

  MOCK_METHOD(VoidAuth, 0, grpc::Status(), void_auth)
};

class MyAuthMock : mock::object {
 public:
  grpc::Status AuthMutVoid(grpc::ServerContext *context, const Request *request,
                           Response *response) {
    return wrap(context, request, response)
        .authorize(this, &MyAuthMock::mut_void_auth)
        .unwrap();
  }

  grpc::Status AuthConstVoid(grpc::ServerContext *context,
                             const Request *request, Response *response) {
    return wrap(context, request, response)
        .authorize(this, &MyAuthMock::const_void_auth)
        .unwrap();
  }

  grpc::Status AuthMutCtx(grpc::ServerContext *context, const Request *request,
                          Response *response) {
    return wrap(context, request, response)
        .authorize(this, &MyAuthMock::mut_context_auth)
        .unwrap();
  }

  grpc::Status AuthConstCtx(grpc::ServerContext *context,
                            const Request *request, Response *response) {
    return wrap(context, request, response)
        .authorize(this, &MyAuthMock::const_context_auth)
        .unwrap();
  }

  grpc::Status AuthMutCtxReq(grpc::ServerContext *context,
                             const Request *request, Response *response) {
    return wrap(context, request, response)
        .authorize(this, &MyAuthMock::mut_context_request_auth)
        .unwrap();
  }

  grpc::Status AuthConstCtxReq(grpc::ServerContext *context,
                               const Request *request, Response *response) {
    return wrap(context, request, response)
        .authorize(this, &MyAuthMock::const_context_request_auth)
        .unwrap();
  }

  MOCK_NON_CONST_METHOD(mut_void_auth, 0, grpc::Status(), mut_void_auth)
  MOCK_CONST_METHOD(const_void_auth, 0, grpc::Status(), const_void_auth)
  MOCK_NON_CONST_METHOD(mut_context_auth, 1,
                        grpc::Status(grpc::ServerContext *), mut_context_auth)
  MOCK_CONST_METHOD(const_context_auth, 1, grpc::Status(grpc::ServerContext *),
                    const_context_auth)
  MOCK_NON_CONST_METHOD(mut_context_request_auth, 2,
                        grpc::Status(grpc::ServerContext *, const Request *),
                        mut_context_request_auth)
  MOCK_CONST_METHOD(const_context_request_auth, 2,
                    grpc::Status(grpc::ServerContext *, const Request *),
                    const_context_request_auth)
};
}  // namespace bunsan::rpc::test

BOOST_AUTO_TEST_SUITE(wrap)

namespace br = bunsan::rpc;
namespace brt = bunsan::rpc::test;

BOOST_AUTO_TEST_CASE(this_auth) {
  grpc::ServerContext *const ctx =
      reinterpret_cast<grpc::ServerContext *>(0x12345);
  brt::Request req;
  brt::Response rsp;
  brt::MyAuthMock mock;
  grpc::Status status;

  MOCK_EXPECT(mock.mut_void_auth)
      .once()
      .returns(grpc::Status(grpc::INVALID_ARGUMENT, "test message"));
  status = mock.AuthMutVoid(ctx, &req, &rsp);
  BOOST_TEST(status.error_code() == grpc::INVALID_ARGUMENT);
  BOOST_TEST(status.error_message() == "test message");

  MOCK_EXPECT(mock.const_void_auth)
      .once()
      .returns(grpc::Status(grpc::NOT_FOUND, "not found"));
  status = mock.AuthConstVoid(ctx, &req, &rsp);
  BOOST_TEST(status.error_code() == grpc::NOT_FOUND);
  BOOST_TEST(status.error_message() == "not found");

  MOCK_EXPECT(mock.mut_context_auth)
      .once()
      .with(ctx)
      .returns(grpc::Status(grpc::INVALID_ARGUMENT, "test message"));
  status = mock.AuthMutCtx(ctx, &req, &rsp);
  BOOST_TEST(status.error_code() == grpc::INVALID_ARGUMENT);
  BOOST_TEST(status.error_message() == "test message");

  MOCK_EXPECT(mock.const_context_auth)
      .once()
      .with(ctx)
      .returns(grpc::Status(grpc::NOT_FOUND, "not found"));
  status = mock.AuthConstCtx(ctx, &req, &rsp);
  BOOST_TEST(status.error_code() == grpc::NOT_FOUND);
  BOOST_TEST(status.error_message() == "not found");

  MOCK_EXPECT(mock.mut_context_request_auth)
      .once()
      .with(ctx, &req)
      .returns(grpc::Status(grpc::INVALID_ARGUMENT, "test message"));
  status = mock.AuthMutCtxReq(ctx, &req, &rsp);
  BOOST_TEST(status.error_code() == grpc::INVALID_ARGUMENT);
  BOOST_TEST(status.error_message() == "test message");

  MOCK_EXPECT(mock.const_context_request_auth)
      .once()
      .with(ctx, &req)
      .returns(grpc::Status(grpc::NOT_FOUND, "not found"));
  status = mock.AuthConstCtxReq(ctx, &req, &rsp);
  BOOST_TEST(status.error_code() == grpc::NOT_FOUND);
  BOOST_TEST(status.error_message() == "not found");
}

MOCK_FUNCTION(free_void_auth, 0, grpc::Status(), free_void_auth)
MOCK_FUNCTION(free_context_auth, 1, grpc::Status(grpc::ServerContext *),
              free_context_auth)
MOCK_FUNCTION(free_context_request_auth, 2,
              grpc::Status(grpc::ServerContext *, const brt::Request *request),
              free_context_request_auth)

BOOST_AUTO_TEST_CASE(free_auth) {
  grpc::ServerContext *const ctx =
      reinterpret_cast<grpc::ServerContext *>(0x12345);
  brt::Request req;
  brt::Response rsp;
  brt::MyAuthMock mock;
  grpc::Status status;

  MOCK_EXPECT(free_void_auth)
      .once()
      .returns(grpc::Status(grpc::NOT_FOUND, "not found"));
  status = br::wrap(ctx, &req, &rsp).authorize(free_void_auth).unwrap();
  BOOST_TEST(status.error_code() == grpc::NOT_FOUND);
  BOOST_TEST(status.error_message() == "not found");

  MOCK_EXPECT(free_context_auth)
      .once()
      .with(ctx)
      .returns(grpc::Status(grpc::INVALID_ARGUMENT, "invalid"));
  status = br::wrap(ctx, &req, &rsp).authorize(free_context_auth).unwrap();
  BOOST_TEST(status.error_code() == grpc::INVALID_ARGUMENT);
  BOOST_TEST(status.error_message() == "invalid");

  MOCK_EXPECT(free_context_request_auth)
      .once()
      .with(ctx, &req)
      .returns(grpc::Status(grpc::CANCELLED, "cancelled"));
  status =
      br::wrap(ctx, &req, &rsp).authorize(free_context_request_auth).unwrap();
  BOOST_TEST(status.error_code() == grpc::CANCELLED);
  BOOST_TEST(status.error_message() == "cancelled");
}

BOOST_AUTO_TEST_CASE(run) {
  grpc::ServerContext *const ctx =
      reinterpret_cast<grpc::ServerContext *>(0x12345);
  brt::Request req;
  brt::Response rsp;
  grpc::Status status;

  status = br::wrap(ctx, &req, &rsp).run([] {}).unwrap();
  BOOST_TEST(status.ok());

  status =
      br::wrap(ctx, &req, &rsp)
          .run([] {
            return grpc::Status(grpc::INVALID_ARGUMENT, "invalid argument");
          })
          .unwrap();
  BOOST_TEST(status.error_code() == grpc::INVALID_ARGUMENT);
  BOOST_TEST(status.error_message() == "invalid argument");

  status = br::wrap(ctx, &req, &rsp)
               .run([] {
                 brt::Response rsp;
                 rsp.set_value(10);
                 return rsp;
               })
               .unwrap();
  BOOST_TEST(status.ok());
  BOOST_TEST(rsp.value() == 10);

  status = br::wrap(ctx, &req, &rsp)
               .run([] { throw std::runtime_error("something bad"); })
               .unwrap();
  BOOST_TEST(status.error_code() == grpc::UNKNOWN);
  BOOST_TEST(status.error_message() == "something bad");
}

BOOST_AUTO_TEST_CASE(chained) {
  grpc::ServerContext *const ctx =
      reinterpret_cast<grpc::ServerContext *>(0x12345);
  brt::Request req;
  brt::Response rsp;
  grpc::Status status;

  MOCK_FUNCTOR(auth, grpc::Status());
  MOCK_FUNCTOR(body, void());

  MOCK_EXPECT(auth).once().returns(grpc::Status::OK);
  MOCK_EXPECT(body).once();
  status = br::wrap(ctx, &req, &rsp).authorize(auth).run(body).unwrap();
  BOOST_TEST(status.ok());

  MOCK_EXPECT(auth).once().returns(grpc::Status::CANCELLED);
  MOCK_EXPECT(body).never();
  status = br::wrap(ctx, &req, &rsp).authorize(auth).run(body).unwrap();
  BOOST_TEST(status.error_code() == grpc::CANCELLED);
}

BOOST_AUTO_TEST_SUITE_END()  // wrap
