#pragma once

#include <bacs/archive/web/content/form/base.hpp>
#include <bacs/archive/web/content/form/widgets/archiver_config.hpp>

namespace bacs::archive::web::content::form {

struct upload : base {
  upload();

  widgets::archiver_config config;
  cppcms::widgets::file archive;
  cppcms::widgets::submit submit;
};

}  // namespace bacs::archive::web::content::form
