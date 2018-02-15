#pragma once

#include <bacs/archive/web/content/form/base.hpp>
#include <bacs/archive/web/content/form/widgets/problem/id.hpp>

namespace bacs::archive::web::content::form {

struct rename : base {
  rename();

  widgets::problem::id current, future;
  cppcms::widgets::submit submit;
};

}  // namespace bacs::archive::web::content::form
