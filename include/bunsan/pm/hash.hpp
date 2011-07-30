#ifndef BUNSAN_PM_HASH_HPP
#define BUNSAN_PM_HASH_HPP

#include <string>

#include <boost/filesystem/path.hpp>

namespace bunsan{namespace pm
{
	std::string hash(const boost::filesystem::path &file);
}}

#endif //BUNSAN_PM_HASH_HPP

