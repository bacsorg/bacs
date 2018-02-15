#pragma once

#include <bunsan/stream_enum.hpp>

#include <cppcms/form.h>

namespace bacs::archive::web::content::form {

struct base : cppcms::form {
  BUNSAN_INCLASS_STREAM_ENUM_CLASS(response_type, (html, protobuf))

  base();

  response_type response();

  cppcms::widgets::select response_;
};

}  // namespace bacs::archive::web::content::form
