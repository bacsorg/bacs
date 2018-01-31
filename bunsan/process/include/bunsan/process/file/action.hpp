#pragma once

#include <boost/filesystem/path.hpp>
#include <boost/variant.hpp>

#include <string>

namespace bunsan {
namespace process {
namespace file {

enum do_default_type { do_default };
enum inherit_type { inherit };
enum suppress_type { suppress };
enum redirect_to_stdout_type { redirect_to_stdout };

using stdin_data_type =
    boost::variant<do_default_type, inherit_type, suppress_type, std::string,
                   boost::filesystem::path>;
using stdout_data_type = boost::variant<do_default_type, inherit_type,
                                        suppress_type, boost::filesystem::path>;
using stderr_data_type =
    boost::variant<do_default_type, inherit_type, suppress_type,
                   redirect_to_stdout_type, boost::filesystem::path>;

}  // namespace file
}  // namespace process
}  // namespace bunsan
