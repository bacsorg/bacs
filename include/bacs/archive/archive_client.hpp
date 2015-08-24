#pragma once

#include <bacs/archive/archive.grpc.pb.h>

#include <grpc++/channel_interface.h>

#include <boost/filesystem/path.hpp>

namespace bacs {
namespace archive {

class ArchiveClient {
 public:
  explicit ArchiveClient(std::shared_ptr<grpc::ChannelInterface> channel);

  grpc::Status Upload(const boost::filesystem::path &path,
                      const ArchiverOptions &archiver,
                      problem::StatusMap &response);
  grpc::Status Download(const DownloadRequest &request,
                        const boost::filesystem::path &path,
                        ArchiverOptions &archiver);

  grpc::Status Rename(const RenameRequest &request,
                      problem::StatusResult &response);

  grpc::Status Existing(const problem::IdSet &request,
                        problem::IdSet &response);
  grpc::Status ExistingAll(const google::protobuf::Empty &request,
                           problem::IdSet &response);

  grpc::Status Status(const problem::IdSet &request,
                      problem::StatusMap &response);
  grpc::Status StatusAll(const google::protobuf::Empty &request,
                         problem::StatusMap &response);

  grpc::Status ImportResult(const problem::IdSet &request,
                            problem::ImportMap &response);

  grpc::Status Import(const problem::IdSet &request,
                      problem::StatusMap &response);
  grpc::Status ImportAll(const google::protobuf::Empty &request,
                         problem::StatusMap &response);

  // Unstable flag API
  grpc::Status WithFlag(const problem::IdSet &request,
                        problem::IdSet &response);
  grpc::Status WithFlagAll(const google::protobuf::Empty &request,
                           problem::IdSet &response);
  grpc::Status SetFlags(const ChangeFlagsRequest &request,
                        problem::StatusMap &response);
  grpc::Status UnsetFlags(const ChangeFlagsRequest &request,
                          problem::StatusMap &response);
  grpc::Status ClearFlags(const problem::IdSet &request,
                          problem::StatusMap &response);

 private:
  std::unique_ptr<Archive::Stub> m_stub;
};

}  // namespace archive
}  // namespace bacs
