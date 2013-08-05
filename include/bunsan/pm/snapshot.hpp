#pragma once

#include <bunsan/pm/entry.hpp>

#include <boost/serialization/access.hpp>
#include <boost/serialization/nvp.hpp>

#include <map>
#include <string>

namespace bunsan{namespace pm
{
    typedef std::string source_name_type;
    typedef std::string checksum_type;
    typedef std::map<source_name_type, checksum_type> snapshot_entry;
    typedef std::map<entry, snapshot_entry> snapshot;
}}
