#pragma once

#include <bacs/archive/archive.grpc.pb.h>

#include <grpc++/grpc++.h>

#include <boost/filesystem/path.hpp>

namespace bacs::archive::rpc {

// FIXME do not rely on grpc::internal, this is a temporary workaround
void recv_file(grpc::internal::ReaderInterface<Chunk> &reader,
               const boost::filesystem::path &path, utility::Archiver &format);
void send_file(const utility::Archiver &format,
               const boost::filesystem::path &path,
               grpc::internal::WriterInterface<Chunk> &writer);

}  // namespace bacs::archive::rpc
