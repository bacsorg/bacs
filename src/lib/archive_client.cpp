#include <bacs/archive/archive_client.hpp>

namespace bacs {
namespace archive {

ArchiveClient::ArchiveClient(std::shared_ptr<grpc::ChannelInterface> channel)
    : m_stub(Archive::NewStub(channel)) {}

grpc::Status ArchiveClient::Upload(const boost::filesystem::path &path,
                                   const ArchiverOptions &archiver,
                                   problem::StatusMap &response) {
  return grpc::Status::CANCELLED;
}

grpc::Status ArchiveClient::Download(const DownloadRequest &request,
                                     const boost::filesystem::path &path,
                                     ArchiverOptions &archiver) {
  return grpc::Status::CANCELLED;
}

grpc::Status ArchiveClient::Rename(const RenameRequest &request,
                                   problem::StatusResult &response) {
  return grpc::Status::CANCELLED;
}

grpc::Status ArchiveClient::Existing(const problem::IdSet &request,
                                     problem::IdSet &response) {
  return grpc::Status::CANCELLED;
}

grpc::Status ArchiveClient::ExistingAll(const google::protobuf::Empty &request,
                                        problem::IdSet &response) {
  return grpc::Status::CANCELLED;
}

grpc::Status ArchiveClient::Status(const problem::IdSet &request,
                                   problem::StatusMap &response) {
  return grpc::Status::CANCELLED;
}

grpc::Status ArchiveClient::StatusAll(const google::protobuf::Empty &request,
                                      problem::StatusMap &response) {
  return grpc::Status::CANCELLED;
}

grpc::Status ArchiveClient::ImportResult(const problem::IdSet &request,
                                         problem::ImportMap &response) {
  return grpc::Status::CANCELLED;
}

grpc::Status ArchiveClient::Import(const problem::IdSet &request,
                                   problem::StatusMap &response) {
  return grpc::Status::CANCELLED;
}

grpc::Status ArchiveClient::ImportAll(const google::protobuf::Empty &request,
                                      problem::StatusMap &response) {
  return grpc::Status::CANCELLED;
}

// Unstable flag API
grpc::Status ArchiveClient::WithFlag(const problem::IdSet &request,
                                     problem::IdSet &response) {
  return grpc::Status::CANCELLED;
}

grpc::Status ArchiveClient::WithFlagAll(const google::protobuf::Empty &request,
                                        problem::IdSet &response) {
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
