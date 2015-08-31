#pragma once

#include <bacs/archive/archive.grpc.pb.h>
#include <bacs/archive/problem.hpp>

#include <grpc++/channel_interface.h>

#include <boost/filesystem/path.hpp>

namespace bacs {
namespace archive {

class ArchiveClient {
 public:
  explicit ArchiveClient(std::shared_ptr<grpc::ChannelInterface> channel);

  grpc::Status Upload(const utility::Archiver &format,
                      const boost::filesystem::path &path,
                      problem::StatusMap &response);
  problem::StatusMap Upload(const utility::Archiver &format,
                            const boost::filesystem::path &path);

  grpc::Status Download(const DownloadRequest &request,
                        const boost::filesystem::path &path,
                        utility::Archiver &format);
  utility::Archiver Download(const DownloadRequest &request,
                             const boost::filesystem::path &path);
  template <typename IdRange>
  utility::Archiver Download(const utility::Archiver &format,
                             const boost::filesystem::path &path,
                             const IdRange &ids) {
    DownloadRequest request;
    *request.mutable_format() = format;
    *request.mutable_ids() = problem::make_id_set(ids);
    return Download(request, path);
  }

  grpc::Status Rename(const RenameRequest &request,
                      problem::StatusResult &response);
  problem::StatusResult Rename(const RenameRequest &request);
  problem::StatusResult Rename(const std::string &from, const std::string &to);

  grpc::Status Existing(const problem::IdSet &request,
                        problem::IdSet &response);
  problem::IdSet Existing(const problem::IdSet &request);
  template <typename IdRange>
  problem::IdSet Existing(const IdRange &ids) {
    return Existing(problem::make_id_set(ids));
  }

  grpc::Status ExistingAll(problem::IdSet &response);
  problem::IdSet ExistingAll();

  grpc::Status Status(const problem::IdSet &request,
                      problem::StatusMap &response);
  problem::StatusMap Status(const problem::IdSet &request);
  template <typename IdRange>
  problem::StatusMap Status(const IdRange &ids) {
    return Status(problem::make_id_set(ids));
  }

  grpc::Status StatusAll(problem::StatusMap &response);
  problem::StatusMap StatusAll();

  grpc::Status ImportResult(const problem::IdSet &request,
                            problem::ImportMap &response);
  problem::ImportMap ImportResult(const problem::IdSet &request);
  template <typename IdRange>
  problem::ImportMap ImportResult(const IdRange &ids) {
    return ImportResult(problem::make_id_set(ids));
  }

  grpc::Status Import(const problem::IdSet &request,
                      problem::StatusMap &response);
  problem::StatusMap Import(const problem::IdSet &request);
  template <typename IdRange>
  problem::StatusMap Import(const IdRange &ids) {
    return Import(problem::make_id_set(ids));
  }

  grpc::Status ImportAll(problem::StatusMap &response);
  problem::StatusMap ImportAll();

  // Unstable flag API
  grpc::Status WithFlag(const problem::IdSet &request,
                        problem::IdSet &response);
  problem::IdSet WithFlag(const problem::IdSet &request);
  grpc::Status WithFlagAll(problem::IdSet &response);
  problem::IdSet WithFlagAll();

  grpc::Status SetFlags(const ChangeFlagsRequest &request,
                        problem::StatusMap &response);
  problem::StatusMap SetFlags(const ChangeFlagsRequest &request);

  grpc::Status UnsetFlags(const ChangeFlagsRequest &request,
                          problem::StatusMap &response);
  problem::StatusMap UnsetFlags(const ChangeFlagsRequest &request);

  grpc::Status ClearFlags(const problem::IdSet &request,
                          problem::StatusMap &response);
  problem::StatusMap ClearFlags(const problem::IdSet &request);

 private:
  std::unique_ptr<Archive::Stub> m_stub;
};

}  // namespace archive
}  // namespace bacs
