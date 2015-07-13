#include <bunsan/protobuf/binary_serializer.hpp>

#include <bunsan/protobuf/error.hpp>

namespace bunsan {
namespace protobuf {

void binary_serializer::serialize_raw(
    const google::protobuf::Message &message,
    google::protobuf::io::ZeroCopyOutputStream &output) {
  if (!message.SerializePartialToZeroCopyStream(&output))
    BOOST_THROW_EXCEPTION(serialize_error()
                          << serialize_error::type_name(message.GetTypeName()));
}

}  // namespace protobuf
}  // namespace bunsan
