#pragma once

#include <bacs/archive/problem.pb.h>

#include <bacs/problem/id.hpp>
#include <bacs/problem/problem.pb.h>

#include <boost/optional.hpp>

#include <string>
#include <unordered_map>
#include <unordered_set>

namespace bacs {
namespace archive {
namespace problem {

using bacs::problem::id;
using bacs::problem::is_allowed_id;
using bacs::problem::validate_id;
using bacs::problem::Revision;
using flag = std::string;

template <typename Range>
IdSet make_id_set(const Range &range) {
  IdSet set;
  for (const problem::id &id : range) set.add_id(id);
  return set;
}

const std::string &flag_cast(const problem::Flag &flag);
const std::string &flag_cast(const problem::Flag::Reserved flag);
Flag flag_cast(const std::string &flag);

const std::string &flag_to_string(const problem::Flag &flag);
const std::string &flag_to_string(const problem::Flag::Reserved flag);
const std::string &flag_to_string(const std::string &flag);

template <typename Flag1, typename Flag2>
bool flag_equal(const Flag1 &flag1, const Flag2 &flag2) {
  return flag_to_string(flag1) == flag_to_string(flag2);
}

bool is_allowed_flag(const flag &flag_);
bool is_allowed_flag(const Flag &flag);
bool is_allowed_flag_set(const FlagSet &flags);

/// \throws invalid_flag_error if !is_allowed_flag(flag_)
void validate_flag(const flag &flag_);
void validate_flag(const Flag &flag);
void validate_flag_set(const FlagSet &flags);

}  // namespace problem
}  // namespace archive
}  // namespace bacs
