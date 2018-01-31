#pragma once

#include <bunsan/crypto/common.hpp>

#include <string>
#include <vector>

namespace bunsan {
namespace crypto {
namespace base64 {

std::string encode(const byte *data, std::size_t size);
std::string decode(const char *data, std::size_t size);

inline std::string encode(const char *data, std::size_t size) {
  return encode(reinterpret_cast<const byte *>(data), size);
}

inline std::string decode(const byte *data, std::size_t size) {
  return decode(reinterpret_cast<const char *>(data), size);
}

inline std::string encode(const std::string &data) {
  return encode(data.data(), data.size());
}

inline std::string encode(const std::vector<byte> &data) {
  return encode(data.data(), data.size());
}

inline std::string decode(const std::string &data) {
  return decode(data.data(), data.size());
}

inline std::string decode(const std::vector<byte> &data) {
  return decode(data.data(), data.size());
}

}  // namespace base64
}  // namespace crypto
}  // namespace bunsan
