#pragma once

#include <bacs/archive/web/content/form/base.hpp>
#include <bacs/archive/web/content/form/widgets/problem/id_set.hpp>

namespace bacs {
namespace archive {
namespace web {
namespace content {
namespace form {

struct list_query : base {
  list_query();
  explicit list_query(const std::string &query);

  widgets::problem::id_set ids;
  cppcms::widgets::submit submit;
};

}  // namespace form
}  // namespace content
}  // namespace web
}  // namespace archive
}  // namespace bacs
