#include <bacs/archive/archive_client.hpp>

#include <bacs/archive/error.hpp>
#include <bacs/archive/rpc.hpp>

#include <bunsan/rpc/overload.hpp>

namespace bacs {
namespace archive {

ArchiveClient::ArchiveClient(std::shared_ptr<grpc::ChannelInterface> channel)
    : m_stub(Archive::NewStub(channel)) {}

grpc::Status ArchiveClient::Upload(const ArchiverOptions &format,
                                   const boost::filesystem::path &path,
                                   problem::StatusMap &response) {
  grpc::ClientContext context;
  const auto writer = m_stub->Upload(&context, &response);
  rpc::send_file(format, path, *writer);
  writer->WritesDone();
  return writer->Finish();
}

problem::StatusMap ArchiveClient::Upload(const ArchiverOptions &format,
                                         const boost::filesystem::path &path) {
  BUNSAN_RPC_OVERLOAD(Upload, problem::StatusMap, rpc_error, format, path)
}

grpc::Status ArchiveClient::Download(const DownloadRequest &request,
                                     const boost::filesystem::path &path,
                                     ArchiverOptions &format) {
  grpc::ClientContext context;
  const auto reader = m_stub->Download(&context, request);
  rpc::recv_file(*reader, path, format);
  return reader->Finish();
}

ArchiverOptions ArchiveClient::Download(const DownloadRequest &request,
                                        const boost::filesystem::path &path) {
  BUNSAN_RPC_OVERLOAD(Download, ArchiverOptions, rpc_error, request, path)
}

grpc::Status ArchiveClient::Rename(const RenameRequest &request,
                                   problem::StatusResult &response) {
  grpc::ClientContext context;
  return m_stub->Rename(&context, request, &response);
}

problem::StatusResult ArchiveClient::Rename(const RenameRequest &request) {
  BUNSAN_RPC_OVERLOAD(Rename, problem::StatusResult, rpc_error, request)
}

problem::StatusResult ArchiveClient::Rename(const std::string &from,
                                            const std::string &to) {
  RenameRequest request;
  request.set_from(from);
  request.set_to(to);
  return Rename(request);
}

grpc::Status ArchiveClient::Existing(const problem::IdSet &request,
                                     problem::IdSet &response) {
  grpc::ClientContext context;
  return m_stub->Existing(&context, request, &response);
}

problem::IdSet ArchiveClient::Existing(const problem::IdSet &request) {
  BUNSAN_RPC_OVERLOAD(Existing, problem::IdSet, rpc_error, request)
}

grpc::Status ArchiveClient::ExistingAll(problem::IdSet &response) {
  grpc::ClientContext context;
  return m_stub->ExistingAll(&context, google::protobuf::Empty(), &response);
}

problem::IdSet ArchiveClient::ExistingAll() {
  BUNSAN_RPC_OVERLOAD(ExistingAll, problem::IdSet, rpc_error)
}

grpc::Status ArchiveClient::Status(const problem::IdSet &request,
                                   problem::StatusMap &response) {
  grpc::ClientContext context;
  return m_stub->Status(&context, request, &response);
}

problem::StatusMap ArchiveClient::Status(const problem::IdSet &request) {
  BUNSAN_RPC_OVERLOAD(Status, problem::StatusMap, rpc_error, request)
}

grpc::Status ArchiveClient::StatusAll(problem::StatusMap &response) {
  grpc::ClientContext context;
  return m_stub->StatusAll(&context, google::protobuf::Empty(), &response);
}

problem::StatusMap ArchiveClient::StatusAll() {
  BUNSAN_RPC_OVERLOAD(StatusAll, problem::StatusMap, rpc_error)
}

grpc::Status ArchiveClient::ImportResult(const problem::IdSet &request,
                                         problem::ImportMap &response) {
  grpc::ClientContext context;
  return m_stub->ImportResult(&context, request, &response);
}

problem::ImportMap ArchiveClient::ImportResult(const problem::IdSet &request) {
  BUNSAN_RPC_OVERLOAD(ImportResult, problem::ImportMap, rpc_error, request)
}

grpc::Status ArchiveClient::Import(const problem::IdSet &request,
                                   problem::StatusMap &response) {
  grpc::ClientContext context;
  return m_stub->Import(&context, request, &response);
}

problem::StatusMap ArchiveClient::Import(const problem::IdSet &request) {
  BUNSAN_RPC_OVERLOAD(Import, problem::StatusMap, rpc_error, request)
}

grpc::Status ArchiveClient::ImportAll(problem::StatusMap &response) {
  grpc::ClientContext context;
  return m_stub->ImportAll(&context, google::protobuf::Empty(), &response);
}

problem::StatusMap ArchiveClient::ImportAll() {
  BUNSAN_RPC_OVERLOAD(ImportAll, problem::StatusMap, rpc_error)
}

// Unstable flag API
grpc::Status ArchiveClient::WithFlag(const problem::IdSet &request,
                                     problem::IdSet &response) {
  return grpc::Status::CANCELLED;
}

grpc::Status ArchiveClient::WithFlagAll(problem::IdSet &response) {
  return grpc::Status::CANCELLED;
}

grpc::Status ArchiveClient::SetFlags(const ChangeFlagsRequest &request,
                                     problem::StatusMap &response) {
  return grpc::Status::CANCELLED;
}

grpc::Status ArchiveClient::UnsetFlags(const ChangeFlagsRequest &request,
                                       problem::StatusMap &response) {
  return grpc::Status::CANCELLED;
}

grpc::Status ArchiveClient::ClearFlags(const problem::IdSet &request,
                                       problem::StatusMap &response) {
  return grpc::Status::CANCELLED;
}

}  // namespace archive
}  // namespace bacs
