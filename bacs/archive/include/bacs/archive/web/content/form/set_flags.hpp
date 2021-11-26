#pragma once

#include <bacs/archive/web/content/form/base.hpp>
#include <bacs/archive/web/content/form/widgets/problem/flag_set.hpp>
#include <bacs/archive/web/content/form/widgets/problem/id_set.hpp>

namespace bacs::archive::web::content::form {

struct set_flags : base {
  set_flags();

  widgets::problem::id_set ids;
  widgets::problem::flag_set flags;
  cppcms::widgets::submit submit;
};

}  // namespace bacs::archive::web::content::form
