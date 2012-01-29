#include "bunsan/utility/resolver.hpp"

#include <boost/process.hpp>

bunsan::utility::resolver::resolver() {}

bunsan::utility::resolver::resolver(const boost::property_tree::ptree &) {}

boost::filesystem::path bunsan::utility::resolver::find_executable(const boost::filesystem::path &exe) const
{
	return boost::process::find_executable_in_path(exe.filename().string());
}

boost::filesystem::path bunsan::utility::resolver::find_library(const boost::filesystem::path &lib) const
{
#warning unimplemented
	return boost::filesystem::path();
}

