#pragma once

#include <bunsan/protobuf/error.hpp>

#include <google/protobuf/message.h>

namespace bunsan::protobuf {

template <typename Error>
const Error &fill_initialization_error(
    const Error &e, const google::protobuf::Message &message) {
  return e << error::message("Uninitialized")
           << error::type_name(message.GetTypeName())
           << error::initialization_error_string(
                  message.InitializationErrorString());
}

template <typename Error = initialization_error>
void check_initialized(const google::protobuf::Message &message) {
  if (!message.IsInitialized())
    BOOST_THROW_EXCEPTION(fill_initialization_error(Error(), message));
}

}  // namespace bunsan::protobuf
