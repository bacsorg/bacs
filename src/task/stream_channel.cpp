#include <bunsan/broker/task/stream_channel.hpp>

#include <bunsan/crypto/base64.hpp>
#include <bunsan/protobuf/binary.hpp>

namespace bunsan {
namespace broker {
namespace task {

stream_channel::stream_channel(std::ostream &out) : m_out(out) {}

void stream_channel::send_status(const Status &status) {
  m_out << "status "
        << crypto::base64::encode(protobuf::binary::to_string(status))
        << std::endl;
}

void stream_channel::send_result(const Result &result) {
  m_out << "result "
        << crypto::base64::encode(protobuf::binary::to_string(result))
        << std::endl;
}

}  // namespace task
}  // namespace broker
}  // namespace bunsan
