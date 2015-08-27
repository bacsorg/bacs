#include <bacs/archive/archive_service.hpp>

namespace bacs {
namespace archive {

ArchiveService::ArchiveService(
    const ArchiveServiceConfig &config,
    const std::shared_ptr<archive::repository> &repository_)
    : m_upload_directory(config.upload_directory()),
      m_repository(repository_) {}

grpc::Status ArchiveService::Upload(grpc::ServerContext *const context,
                                    grpc::ServerReader<Chunk> *const reader,
                                    problem::StatusMap *const response) {
  return grpc::Status::CANCELLED;
}

grpc::Status ArchiveService::Download(grpc::ServerContext *const context,
                                      const DownloadRequest *const request,
                                      grpc::ServerWriter<Chunk> *const writer) {
  return grpc::Status::CANCELLED;
}

grpc::Status ArchiveService::Rename(grpc::ServerContext *const context,
                                    const RenameRequest *const request,
                                    problem::StatusResult *const response) {
  return grpc::Status::CANCELLED;
}

grpc::Status ArchiveService::Existing(grpc::ServerContext *const context,
                                      const problem::IdSet *const request,
                                      problem::IdSet *const response) {
  return grpc::Status::CANCELLED;
}
grpc::Status ArchiveService::ExistingAll(
    grpc::ServerContext *const context,
    const google::protobuf::Empty * /*request*/,
    problem::IdSet *const response) {
  return grpc::Status::CANCELLED;
}

grpc::Status ArchiveService::Status(grpc::ServerContext *const context,
                                    const problem::IdSet *const request,
                                    problem::StatusMap *const response) {
  return grpc::Status::CANCELLED;
}

grpc::Status ArchiveService::StatusAll(
    grpc::ServerContext *const context,
    const google::protobuf::Empty * /*request*/, problem::StatusMap *response) {
  return grpc::Status::CANCELLED;
}

grpc::Status ArchiveService::ImportResult(grpc::ServerContext *const context,
                                          const problem::IdSet *const request,
                                          problem::ImportMap *const response) {
  return grpc::Status::CANCELLED;
}

grpc::Status ArchiveService::Import(grpc::ServerContext *const context,
                                    const problem::IdSet *const request,
                                    problem::StatusMap *const response) {
  return grpc::Status::CANCELLED;
}

grpc::Status ArchiveService::ImportAll(
    grpc::ServerContext *const context,
    const google::protobuf::Empty * /*request*/,
    problem::StatusMap *const response) {
  return grpc::Status::CANCELLED;
}

// Unstable flag API
grpc::Status ArchiveService::WithFlag(grpc::ServerContext *const context,
                                      const problem::IdSet *const request,
                                      problem::IdSet *const response) {
  return grpc::Status::CANCELLED;
}

grpc::Status ArchiveService::WithFlagAll(
    grpc::ServerContext *const context,
    const google::protobuf::Empty * /*request*/,
    problem::IdSet *const response) {
  return grpc::Status::CANCELLED;
}

grpc::Status ArchiveService::SetFlags(grpc::ServerContext *const context,
                                      const ChangeFlagsRequest *const request,
                                      problem::StatusMap *const response) {
  return grpc::Status::CANCELLED;
}

grpc::Status ArchiveService::UnsetFlags(grpc::ServerContext *const context,
                                        const ChangeFlagsRequest *const request,
                                        problem::StatusMap *const response) {
  return grpc::Status::CANCELLED;
}

grpc::Status ArchiveService::ClearFlags(grpc::ServerContext *const context,
                                        const problem::IdSet *const request,
                                        problem::StatusMap *const response) {
  return grpc::Status::CANCELLED;
}

}  // namespace archive
}  // namespace bacs
