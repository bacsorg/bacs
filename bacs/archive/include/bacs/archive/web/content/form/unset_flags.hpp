#pragma once

#include <bacs/archive/web/content/form/base.hpp>
#include <bacs/archive/web/content/form/widgets/problem/flag_set.hpp>
#include <bacs/archive/web/content/form/widgets/problem/id_set.hpp>

namespace bacs {
namespace archive {
namespace web {
namespace content {
namespace form {

struct unset_flags : base {
  unset_flags();

  widgets::problem::id_set ids;
  widgets::problem::flag_set flags;
  cppcms::widgets::submit submit;
};

}  // namespace form
}  // namespace content
}  // namespace web
}  // namespace archive
}  // namespace bacs
