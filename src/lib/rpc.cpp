#include <bacs/archive/rpc.hpp>

#include <bunsan/filesystem/fstream.hpp>

namespace bacs {
namespace archive {
namespace rpc {

void recv_file(grpc::ReaderInterface<Chunk> &reader,
               const boost::filesystem::path &path, ArchiverOptions &format) {
  format.Clear();
  bunsan::filesystem::ofstream fout(path);
  BUNSAN_FILESYSTEM_FSTREAM_WRAP_BEGIN(fout) {
    Chunk chunk;
    while (reader.Read(&chunk)) {
      if (chunk.has_format()) format = chunk.format();
      if (!chunk.data().empty()) {
        fout.write(chunk.data().data(), chunk.data().size());
      }
    }
  } BUNSAN_FILESYSTEM_FSTREAM_WRAP_END(fout)
  fout.close();
}

void send_file(const ArchiverOptions &format,
               const boost::filesystem::path &path,
               grpc::WriterInterface<Chunk> &writer) {
  bunsan::filesystem::ifstream fin(path);
  BUNSAN_FILESYSTEM_FSTREAM_WRAP_BEGIN(fin) {
    Chunk chunk;
    *chunk.mutable_format() = format;
    if (!writer.Write(chunk)) return;
    chunk.Clear();
    // larger buffer for better network utilization, not measured
    char buf[1024 * 1024];
    do {
      fin.read(buf, sizeof(buf));
      chunk.set_data(buf, fin.gcount());
      if (!writer.Write(chunk)) break;
    } while (fin);
  } BUNSAN_FILESYSTEM_FSTREAM_WRAP_END(fin)
  fin.close();
}

}  // namespace rpc
}  // namespace archive
}  // namespace bacs
