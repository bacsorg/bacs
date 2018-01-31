#pragma once

#include <boost/filesystem/path.hpp>

namespace bunsan {
namespace process {
namespace detail {

boost::filesystem::path find_executable_in_path(
    const boost::filesystem::path &executable);

}  // namespace detail
}  // namespace process
}  // namespace bunsan
