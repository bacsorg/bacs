#ifndef BUNSAN_PM_CHECKSUM_HPP
#define BUNSAN_PM_CHECKSUM_HPP

#include <string>

#include <boost/filesystem/path.hpp>

namespace bunsan{namespace pm
{
    std::string checksum(const boost::filesystem::path &file);
}}

#endif //BUNSAN_PM_CHECKSUM_HPP

