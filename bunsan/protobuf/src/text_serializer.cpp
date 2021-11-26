#include <bunsan/protobuf/text_serializer.hpp>

#include <bunsan/protobuf/error.hpp>

#include <google/protobuf/text_format.h>

namespace bunsan::protobuf {

void text_serializer::serialize_raw(
    const google::protobuf::Message &message,
    google::protobuf::io::ZeroCopyOutputStream &output) {
  google::protobuf::TextFormat::Printer printer;
  if (!printer.Print(message, &output))
    BOOST_THROW_EXCEPTION(serialize_error()
                          << serialize_error::type_name(message.GetTypeName()));
}

}  // namespace bunsan::protobuf
