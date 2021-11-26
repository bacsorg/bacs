#pragma once

#include <bacs/archive/archiver_options.hpp>

#include <cppcms/form.h>

namespace bacs::archive::web::content::form::widgets {

struct archiver_config : cppcms::form {
  archiver_config();

  archiver_options value();

  cppcms::widgets::text type;
  cppcms::widgets::text format;
};

}  // namespace bacs::archive::web::content::form::widgets
