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

bool is_allowed_flag(const flag &flag_);

/// \throws invalid_flag_error if !is_allowed_flag(flag_)
void validate_flag(const flag &flag_);

}  // namespace problem
}  // namespace archive
}  // namespace bacs
