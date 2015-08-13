#include <bunsan/pm/checksum.hpp>

#include <bunsan/crypto/checksum.hpp>

namespace bunsan {
namespace pm {

std::string checksum(const boost::filesystem::path &file) {
  return crypto::checksum::sha512(file);
}

}  // namespace pm
}  // namespace bunsan
