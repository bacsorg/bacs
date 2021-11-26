#pragma once

#include <google/protobuf/message.h>

namespace bunsan::protobuf {

bool equal(const google::protobuf::Message &a,
           const google::protobuf::Message &b);

}  // namespace bunsan::protobuf
