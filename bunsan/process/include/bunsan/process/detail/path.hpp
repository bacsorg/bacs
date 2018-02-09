#pragma once

#include <boost/filesystem/path.hpp>

namespace bunsan::process::detail {

boost::filesystem::path find_executable_in_path(
    const boost::filesystem::path &executable);

}  // namespace bunsan::process::detail
