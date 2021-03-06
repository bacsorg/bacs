#pragma once

#include <bunsan/protobuf/base_parser.hpp>

namespace bunsan::protobuf {

class binary_parser : public base_parser {
 protected:
  void merge_raw(google::protobuf::Message &message,
                 google::protobuf::io::ZeroCopyInputStream &input) override;
};

}  // namespace bunsan::protobuf
