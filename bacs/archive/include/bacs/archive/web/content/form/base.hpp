#pragma once

#include <bunsan/stream_enum.hpp>

#include <cppcms/form.h>

namespace bacs {
namespace archive {
namespace web {
namespace content {
namespace form {

struct base : cppcms::form {
  BUNSAN_INCLASS_STREAM_ENUM_CLASS(response_type, (html, protobuf))

  base();

  response_type response();

  cppcms::widgets::select response_;
};

}  // namespace form
}  // namespace content
}  // namespace web
}  // namespace archive
}  // namespace bacs
