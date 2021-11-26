#pragma once

#include <bacs/archive/web/content/form/base.hpp>
#include <bacs/archive/web/content/form/widgets/problem/optional_id_set.hpp>

namespace bacs::archive::web::content::form {

struct import : base {
  import();

  widgets::problem::optional_id_set ids;
  cppcms::widgets::submit submit;
};

}  // namespace bacs::archive::web::content::form
