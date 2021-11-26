#pragma once

#include <bunsan/process/detail/context.hpp>

namespace bunsan::process::detail {

/*!
 * \return Non-negative value if program successfully exited,
 * negative value if program was not able to exit,
 * for UNIX-like it is guaranteed to be -termsig.
 */
int sync_execute(context ctx);

}  // namespace bunsan::process::detail
