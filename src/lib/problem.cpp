#include <bacs/archive/problem.hpp>

#include <bacs/archive/error.hpp>

#include <bunsan/pm/entry.hpp>

namespace bacs {
namespace archive {
namespace problem {

bool is_allowed_flag(const flag &flag_) {
  return bunsan::pm::entry::is_allowed_subpath(flag_);
}

void validate_flag(const flag &flag_) {
  if (!is_allowed_flag(flag_))
    BOOST_THROW_EXCEPTION(invalid_flag_error()
                          << invalid_flag_error::flag(flag_));
}

}  // namespace problem
}  // namespace archive
}  // namespace bacs
