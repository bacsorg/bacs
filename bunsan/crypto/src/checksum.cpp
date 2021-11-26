#include <bunsan/crypto/checksum.hpp>

#include <botan/hash.h>

#include <bunsan/filesystem/fstream.hpp>

#include <memory>
#include <iomanip>
#include <sstream>
#include <string_view>

namespace bunsan::crypto::checksum {

std::string checksum_(const boost::filesystem::path &path, const std::string_view hash) {
  std::unique_ptr<Botan::HashFunction> checksum(Botan::HashFunction::create(hash.data()));

  bunsan::filesystem::ifstream fin(path, std::ios_base::binary);
  BUNSAN_FILESYSTEM_FSTREAM_WRAP_BEGIN(in) {
    char buf[BUFSIZ];
    do {
      fin.read(buf, BUFSIZ);
      checksum->update(reinterpret_cast<Botan::byte *>(buf), fin.gcount());
    } while (fin);
  } BUNSAN_FILESYSTEM_FSTREAM_WRAP_END(fin)
  fin.close();
  std::vector<Botan::byte> result;
  checksum->final(result);
  std::stringstream sout;
  for (const byte b : result) {
    sout << std::hex << std::uppercase << std::setw(2) << std::setfill('0')
         << static_cast<unsigned>(b);
  }
  return sout.str();
}

std::string crc24(const boost::filesystem::path &path) {
  return checksum_(path, "CRC24");
}

std::string crc32(const boost::filesystem::path &path) {
  return checksum_(path, "CRC32");
}

std::string md5(const boost::filesystem::path &path) {
  return checksum_(path, "MD5");
}

std::string sha1(const boost::filesystem::path &path) {
  return checksum_(path, "SHA-160");
}

std::string sha224(const boost::filesystem::path &path) {
  return checksum_(path, "SHA-224");
}

std::string sha256(const boost::filesystem::path &path) {
  return checksum_(path, "SHA-256");
}

std::string sha384(const boost::filesystem::path &path) {
  return checksum_(path, "SHA-384");
}

std::string sha512(const boost::filesystem::path &path) {
  return checksum_(path, "SHA-512");
}

}  // namespace bunsan::crypto::checksum
