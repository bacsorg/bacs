#pragma once

#include <bunsan/crypto/common.hpp>

#include <boost/filesystem/path.hpp>

#include <string>

namespace bunsan {
namespace crypto {
namespace checksum {

std::string crc24(const boost::filesystem::path &path);
std::string crc32(const boost::filesystem::path &path);

std::string md5(const boost::filesystem::path &path);

std::string sha1(const boost::filesystem::path &path);
std::string sha224(const boost::filesystem::path &path);
std::string sha256(const boost::filesystem::path &path);
std::string sha384(const boost::filesystem::path &path);
std::string sha512(const boost::filesystem::path &path);

}  // namespace checksum
}  // namespace crypto
}  // namespace bunsan
