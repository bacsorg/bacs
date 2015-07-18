#pragma once

#include <bacs/archive/archiver_options.hpp>

#include <cppcms/form.h>

namespace bacs {
namespace archive {
namespace web {
namespace content {
namespace form {
namespace widgets {

struct archiver_config : cppcms::form {
  archiver_config();

  archiver_options value();

  cppcms::widgets::text type;
  cppcms::widgets::text format;
};

}  // namespace widgets
}  // namespace form
}  // namespace content
}  // namespace web
}  // namespace archive
}  // namespace bacs
