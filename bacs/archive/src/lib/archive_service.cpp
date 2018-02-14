#include <bacs/archive/archive_service.hpp>

#include <bacs/archive/rpc.hpp>

#include <bunsan/log/trivial.hpp>
#include <bunsan/rpc/implement.hpp>
#include <bunsan/tempfile.hpp>

#include <algorithm>

namespace bacs {
namespace archive {

ArchiveService::ArchiveService(
    const ArchiveServiceConfig &config,
    const std::shared_ptr<archive::repository> &repository_)
    : m_upload_directory(config.upload_directory()),
      m_permissions(config.permissions()),
      m_repository(repository_) {}

namespace {
archiver_options make_archiver_options(const utility::Archiver &archiver) {
  archiver_options config;
  config.type = archiver.type();
  if (!archiver.format().empty()) {
    config.config.put("format", archiver.format());
  }
  return config;
}
}  // namespace

grpc::Status ArchiveService::Upload(grpc::ServerContext *const context,
                                    grpc::ServerReader<Chunk> *const reader,
                                    problem::StatusMap *const response) {
  BUNSAN_RPC_IMPLEMENT_STATUS(context, reader, response, authorize, {
    const auto archive =
        bunsan::tempfile::regular_file_in_directory(m_upload_directory);
    utility::Archiver format;
    rpc::recv_file(*reader, archive.path(), format);
    if (context->IsCancelled()) return grpc::Status::CANCELLED;
    *response = m_repository->upload_all(make_archiver_options(format),
                                         archive.path());
    return grpc::Status::OK;
  })
}

grpc::Status ArchiveService::Download(grpc::ServerContext *const context,
                                      const DownloadRequest *const request,
                                      grpc::ServerWriter<Chunk> *const writer) {
  BUNSAN_RPC_IMPLEMENT_STATUS(context, request, writer, authorize, {
    const auto archive = m_repository->download_all(
        request->ids(), make_archiver_options(request->format()));
    rpc::send_file(request->format(), archive.path(), *writer);
    if (context->IsCancelled()) return grpc::Status::CANCELLED;
    return grpc::Status::OK;
  })
}

grpc::Status ArchiveService::Rename(grpc::ServerContext *const context,
                                    const RenameRequest *const request,
                                    problem::StatusResult *const response) {
  BUNSAN_RPC_IMPLEMENT_RESPONSE(context, request, response, authorize, {
    return m_repository->rename(request->from(), request->to());
  })
}

grpc::Status ArchiveService::Existing(grpc::ServerContext *const context,
                                      const problem::IdSet *const request,
                                      problem::IdSet *const response) {
  BUNSAN_RPC_IMPLEMENT_RESPONSE(context, request, response, authorize, {
    return m_repository->existing(*request);
  })
}

grpc::Status ArchiveService::ExistingAll(grpc::ServerContext *const context,
                                         const google::protobuf::Empty *request,
                                         problem::IdSet *const response) {
  BUNSAN_RPC_IMPLEMENT_RESPONSE(context, request, response, authorize, {
    return m_repository->existing();
  })
}

grpc::Status ArchiveService::Status(grpc::ServerContext *const context,
                                    const problem::IdSet *const request,
                                    problem::StatusMap *const response) {
  BUNSAN_RPC_IMPLEMENT_RESPONSE(context, request, response, authorize, {
    return m_repository->status_all(*request);
  })
}

grpc::Status ArchiveService::StatusAll(grpc::ServerContext *const context,
                                       const google::protobuf::Empty *request,
                                       problem::StatusMap *response) {
  BUNSAN_RPC_IMPLEMENT_RESPONSE(context, request, response, authorize, {
    return m_repository->status_all();
  })
}

grpc::Status ArchiveService::StatusAllIfChanged(grpc::ServerContext *context,
                                                const ArchiveRevision *request,
                                                CachedStatusMap *response) {
  BUNSAN_RPC_IMPLEMENT_RESPONSE(context, request, response, authorize, {
    return m_repository->status_all_if_changed(*request);
  })
}

grpc::Status ArchiveService::ImportResult(grpc::ServerContext *const context,
                                          const problem::IdSet *const request,
                                          problem::ImportMap *const response) {
  BUNSAN_RPC_IMPLEMENT_RESPONSE(context, request, response, authorize, {
    return m_repository->import_result_all(*request);
  })
}

grpc::Status ArchiveService::Import(grpc::ServerContext *const context,
                                    const problem::IdSet *const request,
                                    problem::StatusMap *const response) {
  BUNSAN_RPC_IMPLEMENT_RESPONSE(context, request, response, authorize, {
    return m_repository->import_all(*request);
  })
}

grpc::Status ArchiveService::ImportAll(grpc::ServerContext *const context,
                                       const google::protobuf::Empty *request,
                                       problem::StatusMap *const response) {
  BUNSAN_RPC_IMPLEMENT_RESPONSE(context, request, response, authorize, {
    return m_repository->import_all();
  })
}

// Unstable flag API
grpc::Status ArchiveService::WithFlag(grpc::ServerContext *const context,
                                      const problem::IdSet *const request,
                                      problem::IdSet *const response) {
  return grpc::Status::CANCELLED;
}

grpc::Status ArchiveService::WithFlagAll(grpc::ServerContext *const context,
                                         const google::protobuf::Empty *request,
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

template <typename Request, typename Response>
grpc::Status ArchiveService::authorize(grpc::ServerContext *const context,
                                       Request * /*request*/,
                                       Response * /*response*/) const {
  if (m_permissions.allow_any()) return grpc::Status::OK;
  const auto auth = context->auth_context();
  if (!auth->IsPeerAuthenticated()) {
    return grpc::Status(grpc::UNAUTHENTICATED, "Unauthenticated");
  }
  const auto identity = auth->GetPeerIdentity();
  for (const auto &allowed : m_permissions.allowed_user()) {
    if (std::equal(allowed.identity().begin(), allowed.identity().end(),
                   identity.begin(), identity.end())) {
      return grpc::Status::OK;
    }
  }
  return grpc::Status(grpc::PERMISSION_DENIED, "User not found");
}

}  // namespace archive
}  // namespace bacs
