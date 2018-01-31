#include <bunsan/crypto/base64.hpp>

#include <botan/base64.h>

namespace bunsan {
namespace crypto {
namespace base64 {

std::string encode(const unsigned char *data, const std::size_t size) {
  return Botan::base64_encode(data, size);
}

std::string decode(const char *data, std::size_t size) {
  const auto ret = Botan::base64_decode(data, size);
  return std::string(reinterpret_cast<const char *>(ret.data()), ret.size());
}

}  // namespace base64
}  // namespace crypto
}  // namespace bunsan
