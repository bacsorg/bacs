#pragma once

#include <bacs/archive/web/content/form/base.hpp>
#include <bacs/archive/web/content/form/widgets/problem/optional_id_set.hpp>

namespace bacs {
namespace archive {
namespace web {
namespace content {
namespace form {

struct existing : base {
  existing();

  widgets::problem::optional_id_set ids;
  cppcms::widgets::submit submit;
};

}  // namespace form
}  // namespace content
}  // namespace web
}  // namespace archive
}  // namespace bacs
