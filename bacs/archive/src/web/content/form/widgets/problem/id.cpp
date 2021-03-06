#include <bacs/archive/web/content/form/widgets/problem/id.hpp>

#include <boost/algorithm/string/classification.hpp>
#include <boost/algorithm/string/join.hpp>
#include <boost/algorithm/string/split.hpp>
#include <boost/algorithm/string/trim.hpp>

#include <algorithm>
#include <vector>

namespace bacs::archive::web::content::form::widgets::problem {

archive::problem::id id::value() { return text::value(); }

void id::value(const archive::problem::id &id_) { text::value(id_); }

bool id::validate() {
  if (!text::validate()) return false;
  valid(archive::problem::is_allowed_id(value()));
  return valid();
}

}  // namespace bacs::archive::web::content::form::widgets::problem
