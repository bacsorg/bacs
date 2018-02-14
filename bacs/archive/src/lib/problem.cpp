#include <bacs/archive/problem.hpp>

#include <bacs/archive/error.hpp>

#include <bunsan/pm/entry.hpp>

namespace bacs {
namespace archive {
namespace problem {

const std::string &flag_cast(const problem::Flag &flag) {
  switch (flag.flag_case()) {
    case Flag::kReserved:
      return flag_cast(flag.reserved());
    case Flag::kCustom:
      return flag.custom();
    default:
      BOOST_ASSERT(false);
  }
}

const std::string &flag_cast(const problem::Flag::Reserved flag) {
  return Flag::Reserved_Name(flag);
}

Flag flag_cast(const std::string &flag) {
  Flag result;
  problem::Flag::Reserved reserved;
  if (Flag::Reserved_Parse(flag, &reserved)) {
    result.set_reserved(reserved);
  } else {
    result.set_custom(flag);
  }
  return result;
}

const std::string &flag_to_string(const problem::Flag &flag) {
  return flag_cast(flag);
}

const std::string &flag_to_string(const problem::Flag::Reserved flag) {
  return flag_cast(flag);
}

const std::string &flag_to_string(const std::string &flag) { return flag; }

bool is_allowed_flag(const flag &flag_) {
  return bunsan::pm::entry::is_allowed_subpath(flag_);
}

bool is_allowed_flag(const Flag &flag) {
  return is_allowed_flag(flag_cast(flag));
}

bool is_allowed_flag_set(const FlagSet &flags) {
  for (const Flag &flag : flags.flag()) {
    if (!is_allowed_flag(flag)) return false;
  }
  return true;
}

void validate_flag(const flag &flag_) {
  if (!is_allowed_flag(flag_))
    BOOST_THROW_EXCEPTION(invalid_flag_error()
                          << invalid_flag_error::flag(flag_));
}

void validate_flag(const Flag &flag) {
  validate_flag(flag_cast(flag));
}

void validate_flag_set(const FlagSet &flags) {
  for (const Flag &flag : flags.flag()) validate_flag(flag);
}

}  // namespace problem
}  // namespace archive
}  // namespace bacs
