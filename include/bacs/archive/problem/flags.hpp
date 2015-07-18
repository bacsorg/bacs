#pragma once

namespace bacs {
namespace archive {
namespace problem {
namespace flags {

/// This flag will exclude problem from non-flags and non-status functions.
constexpr const char *ignore = "ignore";

/*!
 * \brief Lock problem.
 *
 * This flag will exclude problem from operations that
 * can alter problem archive or problem id, i.e. user
 * will not be able to replace problem with another,
 * rename or remove it. But it is still possible
 * to change problem's flags.
 */

constexpr const char *locked = "locked";

/*!
 * \brief Hard lock problem.
 *
 * This flag is harder version of \ref locked.
 * It is the same except it does not allow to edit flags.
 * Only administrator can remove this flag manually.
 */
constexpr const char *read_only = "read_only";

}  // namespace flags
}  // namespace problem
}  // namespace archive
}  // namespace bacs
