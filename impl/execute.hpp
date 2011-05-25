#ifndef IMPL_EXECUTE_HPP
#define IMPL_EXECUTE_HPP

#include <string>
#include <vector>

#include <boost/filesystem/path.hpp>

namespace its{namespace impl
{
	int execute(const boost::filesystem::path &cwd, const std::vector<std::string> &args);
}}

#endif //IMPL_EXECUTE_HPP

