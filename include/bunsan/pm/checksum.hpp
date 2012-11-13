#pragma once

#include <string>

#include <boost/filesystem/path.hpp>

namespace bunsan{namespace pm
{
    std::string checksum(const boost::filesystem::path &file);
}}
