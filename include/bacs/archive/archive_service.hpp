#pragma once

#include <bacs/archive/archive.grpc.pb.h>
#include <bacs/archive/repository.hpp>

namespace bacs {
namespace archive {

class ArchiveService : public Archive::Service {
 public:
  ArchiveService(const ArchiveServiceConfig &config,
                 const std::shared_ptr<archive::repository> &repository_);

  grpc::Status Upload(grpc::ServerContext *context,
                      grpc::ServerReader<Chunk> *reader,
                      problem::StatusMap *response) override;
  grpc::Status Download(grpc::ServerContext *context,
                        const DownloadRequest *request,
                        grpc::ServerWriter<Chunk> *writer) override;

  grpc::Status Rename(grpc::ServerContext *context,
                      const RenameRequest *request,
                      problem::StatusResult *response) override;

  grpc::Status Existing(grpc::ServerContext *context,
                        const problem::IdSet *request,
                        problem::IdSet *response) override;
  grpc::Status ExistingAll(grpc::ServerContext *context,
                           const google::protobuf::Empty *request,
                           problem::IdSet *response) override;

  grpc::Status Status(grpc::ServerContext *context,
                      const problem::IdSet *request,
                      problem::StatusMap *response) override;
  grpc::Status StatusAll(grpc::ServerContext *context,
                         const google::protobuf::Empty *request,
                         problem::StatusMap *response) override;
  grpc::Status StatusAllIfChanged(grpc::ServerContext *context,
                                  const ArchiveRevision *request,
                                  CachedStatusMap *response) override;

  grpc::Status ImportResult(grpc::ServerContext *context,
                            const problem::IdSet *request,
                            problem::ImportMap *response) override;

  grpc::Status Import(grpc::ServerContext *context,
                      const problem::IdSet *request,
                      problem::StatusMap *response) override;
  grpc::Status ImportAll(grpc::ServerContext *context,
                         const google::protobuf::Empty *request,
                         problem::StatusMap *response) override;

  // Unstable flag API
  grpc::Status WithFlag(grpc::ServerContext *context,
                        const problem::IdSet *request,
                        problem::IdSet *response) override;
  grpc::Status WithFlagAll(grpc::ServerContext *context,
                           const google::protobuf::Empty *request,
                           problem::IdSet *response) override;
  grpc::Status SetFlags(grpc::ServerContext *context,
                        const ChangeFlagsRequest *request,
                        problem::StatusMap *response) override;
  grpc::Status UnsetFlags(grpc::ServerContext *context,
                          const ChangeFlagsRequest *request,
                          problem::StatusMap *response) override;
  grpc::Status ClearFlags(grpc::ServerContext *context,
                          const problem::IdSet *request,
                          problem::StatusMap *response) override;

 private:
  template <typename Request, typename Response>
  grpc::Status authorize(grpc::ServerContext *context, Request *request,
                         Response *response) const;

  const boost::filesystem::path m_upload_directory;
  const ArchiveServiceConfig::Permissions m_permissions;
  const std::shared_ptr<archive::repository> m_repository;
};

}  // namespace archive
}  // namespace bacs
