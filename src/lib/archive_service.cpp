#include <bacs/archive/archive_service.hpp>

namespace bacs {
namespace archive {

ArchiveService::ArchiveService(
    const ArchiveServiceConfig &config,
    const std::shared_ptr<archive::repository> &repository_)
    : m_upload_directory(config.upload_directory()),
      m_repository(repository_) {}

grpc::Status ArchiveService::Upload(grpc::ServerContext *context,
                                    grpc::ServerReader<Chunk> *reader,
                                    problem::StatusMap *response) {
  return grpc::Status::CANCELLED;
}

grpc::Status ArchiveService::Download(grpc::ServerContext *context,
                                      const DownloadRequest *request,
                                      grpc::ServerWriter<Chunk> *writer) {
  return grpc::Status::CANCELLED;
}

grpc::Status ArchiveService::Rename(grpc::ServerContext *context,
                                    const RenameRequest *request,
                                    problem::StatusResult *response) {
  return grpc::Status::CANCELLED;
}

grpc::Status ArchiveService::Existing(grpc::ServerContext *context,
                                      const problem::IdSet *request,
                                      problem::IdSet *response) {
  return grpc::Status::CANCELLED;
}
grpc::Status ArchiveService::ExistingAll(grpc::ServerContext *context,
                                         const google::protobuf::Empty *request,
                                         problem::IdSet *response) {
  return grpc::Status::CANCELLED;
}

grpc::Status ArchiveService::Status(grpc::ServerContext *context,
                                    const problem::IdSet *request,
                                    problem::StatusMap *response) {
  return grpc::Status::CANCELLED;
}

grpc::Status ArchiveService::StatusAll(grpc::ServerContext *context,
                                       const google::protobuf::Empty *request,
                                       problem::StatusMap *response) {
  return grpc::Status::CANCELLED;
}

grpc::Status ArchiveService::ImportResult(grpc::ServerContext *context,
                                          const problem::IdSet *request,
                                          problem::ImportMap *response) {
  return grpc::Status::CANCELLED;
}

grpc::Status ArchiveService::Import(grpc::ServerContext *context,
                                    const problem::IdSet *request,
                                    problem::StatusMap *response) {
  return grpc::Status::CANCELLED;
}

grpc::Status ArchiveService::ImportAll(grpc::ServerContext *context,
                                       const google::protobuf::Empty *request,
                                       problem::StatusMap *response) {
  return grpc::Status::CANCELLED;
}

// Unstable flag API
grpc::Status ArchiveService::WithFlag(grpc::ServerContext *context,
                                      const problem::IdSet *request,
                                      problem::IdSet *response) {
  return grpc::Status::CANCELLED;
}

grpc::Status ArchiveService::WithFlagAll(grpc::ServerContext *context,
                                         const google::protobuf::Empty *request,
                                         problem::IdSet *response) {
  return grpc::Status::CANCELLED;
}

grpc::Status ArchiveService::SetFlags(grpc::ServerContext *context,
                                      const ChangeFlagsRequest *request,
                                      problem::StatusMap *response) {
  return grpc::Status::CANCELLED;
}

grpc::Status ArchiveService::UnsetFlags(grpc::ServerContext *context,
                                        const ChangeFlagsRequest *request,
                                        problem::StatusMap *response) {
  return grpc::Status::CANCELLED;
}

grpc::Status ArchiveService::ClearFlags(grpc::ServerContext *context,
                                        const problem::IdSet *request,
                                        problem::StatusMap *response) {
  return grpc::Status::CANCELLED;
}

}  // namespace archive
}  // namespace bacs
