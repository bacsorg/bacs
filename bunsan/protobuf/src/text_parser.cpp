#include <bunsan/protobuf/text_parser.hpp>

#include <bunsan/protobuf/error.hpp>

#include <google/protobuf/text_format.h>

namespace bunsan {
namespace protobuf {

void text_parser::merge_raw(google::protobuf::Message &message,
                            google::protobuf::io::ZeroCopyInputStream &input) {
  google::protobuf::TextFormat::Parser parser;
  parser.AllowPartialMessage(true);
  if (!parser.Merge(&input, &message))
    BOOST_THROW_EXCEPTION(parse_error()
                          << parse_error::type_name(message.GetTypeName()));
}

}  // namespace protobuf
}  // namespace bunsan
