#pragma once

#include <bacs/archive/web/content/form/base.hpp>
#include <bacs/archive/web/content/form/widgets/archiver_config.hpp>
#include <bacs/archive/web/content/form/widgets/problem/id_set.hpp>

namespace bacs::archive::web::content::form {

struct download : base {
  download();

  widgets::archiver_config config;
  widgets::problem::id_set ids;
  cppcms::widgets::submit submit;
};

}  // namespace bacs::archive::web::content::form
