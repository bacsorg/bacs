#include <bacs/archive/web/content/form/widgets/problem/flag_set.hpp>

#include <boost/algorithm/string/classification.hpp>
#include <boost/algorithm/string/join.hpp>
#include <boost/algorithm/string/split.hpp>
#include <boost/algorithm/string/trim.hpp>

#include <algorithm>
#include <vector>
#include <unordered_set>

namespace bacs {
namespace archive {
namespace web {
namespace content {
namespace form {
namespace widgets {
namespace problem {

archive::problem::FlagSet flag_set::value() {
  archive::problem::FlagSet set;
  archive::problem::flag value_(text::value());
  boost::algorithm::trim(value_);
  std::unordered_set<std::string> uset;
  if (!value_.empty())
    boost::algorithm::split(uset, value_, boost::algorithm::is_space(),
                            boost::algorithm::token_compress_on);
  set.clear_flag();
  for (const auto &flag : uset)
    *set.add_flag() = archive::problem::flag_cast(flag);
  return set;
}

void flag_set::value(const archive::problem::FlagSet &flag_set_) {
  std::vector<archive::problem::flag> flags;
  for (const archive::problem::Flag &flag : flag_set_.flag()) {
    flags.push_back(archive::problem::flag_cast(flag));
  }
  std::sort(flags.begin(), flags.end());
  text::value(boost::algorithm::join(flags, " "));
}

bool flag_set::validate() {
  if (!text::validate()) return false;
  valid(archive::problem::is_allowed_flag_set(value()));
  return valid();
}

}  // namespace problem
}  // namespace widgets
}  // namespace form
}  // namespace content
}  // namespace web
}  // namespace archive
}  // namespace bacs
