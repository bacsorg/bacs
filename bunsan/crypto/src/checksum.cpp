#include <bunsan/crypto/checksum.hpp>

#include <botan/crc24.h>
#include <botan/crc32.h>
#include <botan/md5.h>
#include <botan/sha160.h>
#include <botan/sha2_32.h>
#include <botan/sha2_64.h>

#include <bunsan/filesystem/fstream.hpp>

#include <iomanip>
#include <sstream>

namespace bunsan {
namespace crypto {
namespace checksum {

template <typename Checksum>
std::string checksum_(const boost::filesystem::path &path) {
  Checksum checksum;
  bunsan::filesystem::ifstream fin(path, std::ios_base::binary);
  BUNSAN_FILESYSTEM_FSTREAM_WRAP_BEGIN(in) {
    char buf[BUFSIZ];
    do {
      fin.read(buf, BUFSIZ);
      checksum.update(reinterpret_cast<Botan::byte *>(buf), fin.gcount());
    } while (fin);
  } BUNSAN_FILESYSTEM_FSTREAM_WRAP_END(fin)
  fin.close();
  std::vector<Botan::byte> result;
  checksum.final(result);
  std::stringstream sout;
  for (const byte b : result) {
    sout << std::hex << std::uppercase << std::setw(2) << std::setfill('0')
         << static_cast<unsigned>(b);
  }
  return sout.str();
}

std::string crc24(const boost::filesystem::path &path) {
  return checksum_<Botan::CRC24>(path);
}

std::string crc32(const boost::filesystem::path &path) {
  return checksum_<Botan::CRC32>(path);
}

std::string md5(const boost::filesystem::path &path) {
  return checksum_<Botan::MD5>(path);
}

std::string sha1(const boost::filesystem::path &path) {
  return checksum_<Botan::SHA_160>(path);
}

std::string sha224(const boost::filesystem::path &path) {
  return checksum_<Botan::SHA_224>(path);
}

std::string sha256(const boost::filesystem::path &path) {
  return checksum_<Botan::SHA_256>(path);
}

std::string sha384(const boost::filesystem::path &path) {
  return checksum_<Botan::SHA_384>(path);
}

std::string sha512(const boost::filesystem::path &path) {
  return checksum_<Botan::SHA_512>(path);
}

}  // namespace checksum
}  // namespace crypto
}  // namespace bunsan
