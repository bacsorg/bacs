#pragma once

#include <bacs/archive/archive.grpc.pb.h>

#include <grpc++/stream.h>

#include <boost/filesystem/path.hpp>

namespace bacs {
namespace archive {
namespace rpc {

void recv_file(grpc::ReaderInterface<Chunk> &reader,
               const boost::filesystem::path &path, ArchiverOptions &format);
void send_file(const ArchiverOptions &format,
               const boost::filesystem::path &path,
               grpc::WriterInterface<Chunk> &writer);

}  // namespace rpc
}  // namespace archive
}  // namespace bacs
